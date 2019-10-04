import gdb.printing
import traceback
import decimal

codes = {}
codes["Spce"] = " "
codes["Bang"] = "!"
codes["Quot"] = "\""
codes["Hash"] = "#"
codes["Dolr"] = "$"
codes["Perc"] = "%"
codes["And-"] = "&"
codes["Apos"] = "'"
codes["LPar"] = "("
codes["RPar"] = ")"
codes["Star"] = "*"
codes["Plus"] = "+"
codes["Comm"] = ","
codes["Hyph"] = "-"
codes["Stop"] = "."
codes["Slsh"] = "/"
codes["Coln"] = ":"
codes["SCln"] = "l"
codes["-LT-"] = "<"
codes["Eqls"] = "="
codes["-GT-"] = ">"
codes["Ques"] = "?"
codes["-AT-"] = "@"
codes["LSqB"] = "["
codes["RSqB"] = "]"
codes["Bash"] = "\\"
codes["Xor-"] = "^"
codes["Unds"] = "_"
codes["BQuo"] = "`"
codes["LBra"] = "{"
codes["Pipe"] = "|"
codes["RBra"] = "}"
codes["Tild"] = "~"

MAX = 1 << 64 - 1
BL = 5
B = 5
BRANCHES = 32
MASK = 31
MAX_DEPTH=14

MPFR_EXP_MAX = 0x7fffffffffffffff
MPFR_EXP_NAN = 1 - MPFR_EXP_MAX
MPFR_EXP_ZERO = 0 - MPFR_EXP_MAX
MPFR_EXP_INF = 2 - MPFR_EXP_MAX

class Relaxed:
    def __init__(self, node, shift, relaxed, it):
        self.node = node
        self.shift = shift
        self.relaxed = relaxed
        self.it = it

    def index(self, idx):
        offset = idx >> self.shift
        while self.relaxed.dereference()['d']['sizes'][offset] <= idx: offset += 1
        return offset

    def towards(self, idx):
        offset = self.index(idx)
        left_size = self.relaxed.dereference()['d']['sizes'][offset-1] if offset else 0
        child = self.it.inner(self.node)[offset]
        is_leaf = self.shift == BL
        next_size = self.relaxed.dereference()['d']['sizes'][offset] - left_size
        next_idx = idx - left_size
        if is_leaf:
            return self.it.visit_leaf(LeafSub(child, next_size), next_idx)
        else:
            return self.it.visit_maybe_relaxed_sub(child, self.shift - B, next_size, next_idx)

class LeafSub:
    def __init__(self, node, count):
        self.node = node
        self.count_ = count

    def index(self, idx):
        return idx & MASK

    def count(self):
        return self.count_

class FullLeaf:
    def __init__(self, node):
        self.node = node

    def index(self, idx):
        return idx & MASK

    def count(self):
        return BRANCHES

class Leaf:
    def __init__(self, node, size):
        self.node = node
        self.size = size

    def index(self, idx):
        return idx & MASK

    def count(self):
        return self.index(self.size - 1) + 1

class RegularSub:
    def __init__(self, node, shift, size, it):
        self.node = node
        self.shift = shift
        self.size = size
        self.it = it

    def towards(self, idx):
        offset = self.index(idx)
        count = self.count()
        return self.it.towards_regular(self, idx, offset, count)

    def index(self, idx):
        return (idx >> self.shift) & MASK

    def count(self):
        return self.subindex(self.size - 1) + 1

    def subindex(self, idx):
        return idx >> self.shift

class Regular:
    def __init__(self, node, shift, size, it):
        self.node = node
        self.shift = shift
        self.size = size
        self.it = it

    def index(self, idx):
        return (idx >> self.shift) & MASK

    def count(self):
        return self.index(self.size - 1) + 1

    def towards(self, idx):
        offset = self.index(idx)
        count = self.count()
        return self.it.towards_regular(self, idx, offset, count)

class Full:
    def __init__(self, node, shift, it):
        self.node = node
        self.shift = shift
        self.it = it

    def index(self, idx):
        return (idx >> self.shift) & MASK

    def towards(self, idx):
        offset = self.index(idx)
        is_leaf = self.shift == BL
        child = self.it.inner(self.node)[offset]
        if is_leaf:
            return self.it.visit_leaf(FullLeaf(child), idx)
        else:
            return Full(child, self.shift - B, it).towards(idx)

class ListIter:
    def __init__(self, val):
        self.v = val.dereference()['impl_']
        self.size = self.v['size']
        self.i = 0
        self.curr = (None, MAX, MAX)
        self.node_ptr_ptr = self.v['root'].type.pointer()

    def __iter__(self):
        return self

    def __next__(self):
        if self.i == self.size:
            raise StopIteration
        if self.i < self.curr[1] or self.i >= self.curr[2]:
            self.curr = self.region()
        self.i += 1
        return self.curr[0][self.i-1-self.curr[1]]

    def region(self):
        tail_off = self.tail_offset()
        if self.i >= tail_off:
            return (self.leaf(self.v['tail']), tail_off, self.size)
        else:
            subs = self.visit_maybe_relaxed_sub(self.v['root'], self.v['shift'], tail_off, self.i)
            first = self.i - subs[1]
            end = first + subs[2]
            return (subs[0], first, end)

    def tail_offset(self):
        r = self.relaxed(self.v['root'])
        if r:
            return r.dereference()['d']['sizes'][r.dereference()['d']['count'] - 1]
        elif self.size:
            return (self.size - 1) & ~MASK
        else:
            return 0

    def relaxed(self, node):
        return node.dereference()['impl']['d']['data']['inner']['relaxed']

    def leaf(self, node):
        return node.dereference()['impl']['d']['data']['leaf']['buffer'].address

    def inner(self, node):
        return node.dereference()['impl']['d']['data']['inner']['buffer'].address.reinterpret_cast(self.node_ptr_ptr)

    def visit_maybe_relaxed_sub(self, node, shift, size, idx):
        relaxed = self.relaxed(node)
        if relaxed:
            return Relaxed(node, shift, relaxed, self).towards(idx)
        else:
            return RegularSub(node, shift, size, self).towards(idx)

    def visit_leaf(self, pos, idx):
        return (self.leaf(pos.node), pos.index(idx), pos.count())

    def towards_regular(self, pos, idx, offset, count):
        is_leaf = pos.shift == BL
        child = self.inner(pos.node)[offset]
        is_full = offset + 1 != count
        if is_full:
            if is_leaf:
                return self.visit_leaf(FullLeaf(child), idx)
            else:
                return Full(child, pos.shift - B, self).towards(idx)
        elif is_leaf:
            return self.visit_leaf(Leaf(child, pos.size), idx)
        else:
            return Regular(child, pos.shift - B, pos.size, self).towards(idx)

def popcount(x):
    b = 0
    while x > 0:
        x &= x - 1
        b += 1
    return b

class ChampIter:
    def __init__(self, val):
        self.depth = 0
        v = val.dereference()['impl_']['root']
        self.node_ptr_ptr = v.type.pointer()
        m = self.datamap(v)
        if m:
            self.cur = self.values(v)
            self.end = self.values(v) + popcount(m)
        else:
            self.cur = None
            self.end = None
        self.path = [v.address]
        self.ensure_valid()

    def __iter__(self):
        return self

    def __next__(self):
        if self.cur == None:
            raise StopIteration
        result = self.cur.dereference()
        self.cur += 1
        self.ensure_valid()
        return result

    def ensure_valid(self):
        while self.cur == self.end:
            while self.step_down():
                if self.cur != self.end:
                    return
            if not self.step_right():
                self.cur = None
                self.end = None
                return

    def step_down(self):
        if self.depth < MAX_DEPTH:
            parent = self.path[self.depth].dereference()
            if self.nodemap(parent):
                self.depth += 1
                self.path.append(self.children(parent))
                child = self.path[self.depth]
                if self.depth < MAX_DEPTH:
                    m = self.datamap(child)
                    if m:
                        self.cur = self.values(child)
                        self.end = self.cur + popcount(m)
                else:
                    self.cur = self.collisions(child)
                    self.end = self.cur = self.collision_count(child)
                return True
        return False

    def step_right(self):
        while self.depth > 0:
            parent = self.path[self.depth - 1].dereference()
            last = self.children(parent) + popcount(self.nodemap(parent))
            next_ = self.path[self.depth] + 1
            if next_ < last:
                self.path[self.depth] = next_
                child = self.path[self.depth].dereference()
                if self.depth < MAX_DEPTH:
                    m = self.datamap(child)
                    if m:
                        self.cur = self.values(child)
                        self.end = self.cur + popcount(m)
                else:
                    self.cur = self.collisions(child)
                    self.end = self.cur + self.collision_count(child)
                return True
            self.depth -= 1
            self.path.pop()
        return False

    def values(self, node):
        return node.dereference()['impl']['d']['data']['inner']['values'].dereference()['d']['buffer'].address.cast(self.T_ptr)

    def children(self, node):
        return node.dereference()['impl']['d']['data']['inner']['buffer'].address.cast(self.node_ptr_ptr)

    def datamap(self, node):
        return node.dereference()['impl']['d']['data']['inner']['datamap']

    def nodemap(self, node):
        return node.dereference()['impl']['d']['data']['inner']['nodemap']

    def collision_count(self, node):
        return node.dereference()['impl']['d']['data']['collision']['count']

    def collisions(self, node):
        return node.dereference()['impl']['d']['data']['collision']['buffer'].address.cast(self.T_ptr)

class MapIter(ChampIter):
    def __init__(self, val):
        self.T_ptr = gdb.lookup_type("std::pair<KElem, KElem>").pointer()
        ChampIter.__init__(self, val)

class SetIter(ChampIter):
    def __init__(self, val):
        self.T_ptr = gdb.lookup_type("KElem").pointer()
        ChampIter.__init__(self, val)


def mapCode(code):
    try:
        i = int(code, 16)
        return "\\u" + code
    except ValueError:
        return codes[code]

def pp_label(label):
    if label.startswith('Lbl'):
        label = label[3:]
    literal = True
    result = ""
    i = 0
    while i < len(label):
        if label[i] == '\'':
            literal = not literal
            i += 1
        elif literal and label[i] == '{':
            result += label[i:].replace('{Sort','{').replace(' Sort',' ')
            break
        elif literal:
            result += label[i]
            i += 1
        else:
            code = label[i:i+4]
            result += mapCode(code)
            i += 4
    return "`" + result + "`"

class termPrinter:
    """Print a kore term."""

    def __init__(self, val, cat):
        self.val = val
        self.cat = cat
        self.long_int = gdb.lookup_type("long int")
        self.bool_ptr = gdb.lookup_type("bool").pointer()
        self.unsigned_char = gdb.lookup_type("unsigned char")
        self.string_ptr = gdb.lookup_type("string").pointer()
        self.stringbuffer_ptr = gdb.lookup_type("stringbuffer").pointer()
        self.stringbuffer_ptr_ptr = gdb.lookup_type("stringbuffer").pointer().pointer()
        self.block_ptr = gdb.lookup_type("block").pointer()
        self.block_ptr_ptr = gdb.lookup_type("block").pointer().pointer()
        self.mpz_ptr = gdb.lookup_type("__mpz_struct").pointer()
        self.mpz_ptr_ptr = gdb.lookup_type("__mpz_struct").pointer().pointer()
        self.floating_ptr = gdb.lookup_type("floating").pointer()
        self.floating_ptr_ptr = gdb.lookup_type("floating").pointer().pointer()
        self.map_ptr = gdb.lookup_type("map").pointer()
        self.list_ptr = gdb.lookup_type("list").pointer()
        self.set_ptr = gdb.lookup_type("set").pointer()

    def getSymbolNameForTag(self, tag):
        return gdb.lookup_global_symbol("table_getSymbolNameForTag").value()[tag]

    def isSymbolABinder(self, tag):
        return gdb.lookup_global_symbol("table_isSymbolABinder").value()[tag]

    def getLayoutData(self, layout):
        return gdb.lookup_global_symbol("layout_" + str(layout)).value()

    def to_string(self):
        try:
            self.bound_variables = []
            self.var_counter = 0
            self.result = ""
            if self.cat == "block":
                self.append(self.val, False)
            elif self.cat == "list":
                self.appendList(self.val.cast(self.list_ptr))
            elif self.cat == "set":
                self.appendSet(self.val.cast(self.set_ptr))
            elif self.cat == "map":
                self.appendMap(self.val.cast(self.map_ptr))
            elif self.cat == "int":
                self.appendInt(self.val.cast(self.mpz_ptr))
            elif self.cat == "floating":
                self.appendFloat(self.val.cast(self.floating_ptr))
            elif self.cat == "stringbuffer":
                self.appendStringBuffer(self.val.cast(self.stringbuffer_ptr))
            self.var_names = {}
            self.used_var_name = set()
            return self.result
        except:
             print(traceback.format_exc())
             raise

    def appendFloat(self, val):
        mpfr = val.dereference()['f'][0]
        prec = int(mpfr['_mpfr_prec'])
        expBits = int(val.dereference()['exp'])
        if prec == 53 and expBits == 11:
            suffix = ""
        elif prec == 24 and expBits == 8:
            suffix = "f"
        else:
            suffix = "p" + str(prec) + "x" + str(expBits)
        exp = int(mpfr['_mpfr_exp'])
        sign = mpfr['_mpfr_sign'] < 0
        self.result += "#token(\""
        if exp == MPFR_EXP_NAN:
            self.result += "NaN" + suffix
        elif exp == MPFR_EXP_INF:
            if sign:
                self.result += "-Infinity" + suffix
            else:
                self.result += "Infinity" + suffix
        elif exp == MPFR_EXP_ZERO:
            if sign:
                self.result += "-0.0" + suffix
            else:
                self.result += "0.0" + suffix
        else:
            accum = 0
            nlimbs = (prec + 63) // 64
            ptr = mpfr['_mpfr_d']
            for i in range(nlimbs-1,-1,-1):
                accum <<= 64
                limb = int(ptr[i])
                accum |= limb
            accum >>= 64 - prec % 64
            numerator = accum
            denominator = 1 << prec
            if exp >= 0:
                numerator <<= exp
            else:
                denominator <<= -exp
            with decimal.localcontext() as ctx:
                ctx.prec = prec
                dec = decimal.Decimal(numerator)/denominator
                string = str(dec).lower()
                if not string.count('.'):
                    string += ".0"
                self.result += string + suffix
        self.result += "\",\"Float\")"


    def appendStringBuffer(self, val):
        string = val.dereference()['contents'].dereference()['data'].string("iso-8859-1")
        self.result += "#token(\"" + string + "\",\"StringBuffer\")"

    def appendLimbs(self, size, ptr):
        accum = 0
        for i in range(size-1,-1,-1):
            accum <<= 64
            limb = int(ptr[i])
            accum |= limb
        self.result += str(accum)

    def appendInt(self, val):
        self.result += "#token(\""
        size = int(val.dereference()['_mp_size'])
        if size == 0:
            self.result += "0"
        elif size < 0:
            self.result += "-"
            self.appendLimbs(-size, val.dereference()['_mp_d'])
        else:
            self.appendLimbs(size, val.dereference()['_mp_d'])
        self.result += "\", \"Int\")"

    def appendList(self, val):
        length = val.dereference()['impl_']['size']
        if length == 0:
            self.result += "`.List`(.KList)"
            return
        i = 1
        for elem in ListIter(val):
            if i < length:
                self.result += "`_List_`("
            self.result += "`ListItem`("
            self.append(elem.cast(self.block_ptr), False)
            self.result += ")"
            if i < length:
                self.result += ","
            i += 1
        for i in range(length-1):
            self.result += ")"

    def appendMap(self, val):
        length = val.dereference()['impl_']['size']
        if length == 0:
            self.result += "`.Map`(.KList)"
            return
        i = 1
        for entry in MapIter(val):
            key = entry['first']['elem']
            value = entry['second']['elem']
            if i < length:
                self.result += "`_Map_`("
            self.result += "`_|->_`("
            self.append(key.cast(self.block_ptr), False)
            self.result += ","
            self.append(value.cast(self.block_ptr), False)
            self.result += ")"
            if i < length:
                self.result += ","
            i += 1
        for i in range(length-1):
            self.result += ")"

    def appendSet(self, val):
        length = val.dereference()['impl_']['size']
        if length == 0:
            self.result += "`.Set`(.KList)"
            return
        i = 1
        for entry in SetIter(val):
            elem = entry['elem']
            if i < length:
                self.result += "`_Set_`("
            self.result += "`SetItem`("
            self.append(elem.cast(self.block_ptr), False)
            self.result += ")"
            if i < length:
                self.result += ","
            i += 1
        for i in range(length-1):
            self.result += ")"

    def append(self, subject, isVar):
        address = int(subject.cast(self.long_int))
        isConstant = address & 3
        if isConstant:
            tag = address >> 32
            if isConstant == 3:
                self.append(self.bound_variables[len(self.bound_variables)-1-tag], True)
                return
            symbol = self.getSymbolNameForTag(tag).string()
            self.result += symbol + "(.KList)"
            return
        hdr = int(subject.dereference()['h']['hdr'])
        layout = hdr >> @LAYOUT_OFFSET@
        if not layout:
            string = subject.cast(self.string_ptr)
            length = hdr & 0xffffffffff
            self.result += "#token(\""
            for i in range(length):
                c = chr(int(string.dereference()['data'][i].cast(self.unsigned_char)))
                if c == '\\':
                    self.result += "\\\\"
                elif c == '"':
                    self.result += "\\\""
                elif c == '\n':
                    self.result += "\\n"
                elif c == '\t':
                    self.result += "\\t"
                elif c == '\r':
                    self.result += "\\r"
                elif c == '\f':
                    self.result += "\\f"
                elif ord(c) >= 32 and ord(c) < 127:
                    self.result += c
                else:
                    self.result += "{:02x}".format(ord(c))
            stdStr = string.dereference()['data'].string("iso-8859-1")
            if isVar and not stdStr in self.var_names:
                suffix = ""
                while stdStr + suffix in self.used_var_names:
                    suffix = str(self.var_counter)
                    self.var_counter += 1
                oldStdStr = stdStr
                stdStr = stdStr + suffix
                self.result += suffix
                self.used_var_names.append(stdStr)
                self.var_names[oldStdStr] = suffix
            elif isVar:
                self.result += self.var_names[stdStr]
            self.result += "\",\"Id\")"
            return
        tag = hdr & 0xffffffff
        isBinder = self.isSymbolABinder(tag)
        if isBinder:
            self.bound_variables.append((subject.cast(self.long_int) + 8).cast(self.block_ptr_ptr).dereference())
        symbol = self.getSymbolNameForTag(tag).string()
        self.result += pp_label(symbol) + "("
        layoutData = self.getLayoutData(layout)
        nargs = int(layoutData['nargs'])
        for i in range(nargs):
            argData = layoutData['args'] + i
            arg = subject.cast(self.long_int) + int(argData.dereference()['offset'])
            cat = argData.dereference()['cat']
            if cat == @MAP_LAYOUT@:
                self.appendMap(arg.cast(self.map_ptr))
            elif cat == @LIST_LAYOUT@:
                self.appendList(arg.cast(self.list_ptr))
            elif cat == @SET_LAYOUT@:
                self.appendSet(arg.cast(self.set_ptr))
            elif cat == @SYMBOL_LAYOUT@:
                self.append(arg.cast(self.block_ptr_ptr).dereference(), False)
            elif cat == @VARIABLE_LAYOUT@:
                self.append(arg.cast(self.block_ptr_ptr).dereference(), True)
            elif cat == @INT_LAYOUT@:
                self.appendInt(arg.cast(self.mpz_ptr_ptr).dereference())
            elif cat == @FLOAT_LAYOUT@:
                self.appendFloat(arg.cast(self.floating_ptr_ptr).dereference())
            elif cat == @BOOL_LAYOUT@:
                string = "true" if arg.cast(self.bool_ptr).dereference() else "false"
                self.result += "#token(\"" + string + "\",\"Bool\")"
            elif cat == @STRINGBUFFER_LAYOUT@:
                self.appendStringBuffer(arg.cast(self.stringbuffer_ptr_ptr).dereference())
            else:
                raise ValueError()
            if i != nargs - 1:
                self.result += ","
        if isBinder:
            self.bound_variables.pop()
        self.result += ")"

def kllvm_lookup_function(val):
    t = gdb.types.get_basic_type(val.type)
    if t.code == gdb.TYPE_CODE_PTR and t.target().tag:
        if t.target().tag == "block":
            return termPrinter(val, "block")
        elif t.target().tag == "list":
            return termPrinter(val, "list")
        elif t.target().tag == "map":
            return termPrinter(val, "map")
        elif t.target().tag == "set":
            return termPrinter(val, "set")
        elif t.target().tag == "stringbuffer":
            return termPrinter(val, "stringbuffer")
        elif t.target().tag == "__mpz_struct":
            return termPrinter(val, "int")
        elif t.target().tag == "floating":
            return termPrinter(val, "floating")
    return None
