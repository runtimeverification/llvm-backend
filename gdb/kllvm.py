import gdb.printing
import traceback
import decimal
import subprocess
import sys

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


class Variable:
    def __init__(self, val):
        self.stdStr = val.dereference()['data'].string("iso-8859-1")
        self.var_bit = int(val.dereference()['h']['hdr'] & @VARIABLE_BIT@)
        self.address = int(val)

    def __eq__(self, other):
        if self.var_bit and other.var_bit:
            return self.address == other.address
        return self.stdStr == other.stdStr

    def __hash__(self):
        return hash(self.stdStr)

    def __str__(self):
        return "Variable" + repr((self.stdStr, self.var_bit, self.address))

class KParam(gdb.Parameter):
    def __init__(self, name, value, showDoc, setDoc):
        super(KParam, self).__init__(name, gdb.COMMAND_DATA, gdb.PARAM_BOOLEAN)
        self.value = value
        self.show_doc = showDoc
        self.set_doc = setDoc

hasColor = KParam("print k-color", True, "Coloring of pretty-printed k terms is", "Set coloring of pretty-printed k terms")
prettyPrint = KParam("print k-pretty", True, "Pretty-printing of k terms is", "Set pretty-printing of k terms")

def getKompiledDir():
  return gdb.lookup_global_symbol("kompiled_directory").value().address.string("iso-8859-1")

def printKore(string, kompiledDir): 
    if prettyPrint.value:
        return str(subprocess.check_output(["kprint", kompiledDir, "/dev/stdin", "true" if hasColor.value else "false"], input=bytes(string, "iso-8859-1")), "iso-8859-1")[:-1]
    else:
        return string
 

class termPrinter:
    """Print a kore term."""

    def __init__(self, val, cat, sortName):
        self.val = val
        self.cat = cat
        self.sortName = sortName
        self.var_names = {}
        self.used_var_names = set()
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
        self.kompiled_dir = getKompiledDir()

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
                self.append(self.val, False, self.sortName)
            elif self.cat == "list":
                self.appendList(self.val.cast(self.list_ptr), self.sortName)
            elif self.cat == "set":
                self.appendSet(self.val.cast(self.set_ptr), self.sortName)
            elif self.cat == "map":
                self.appendMap(self.val.cast(self.map_ptr), self.sortName)
            elif self.cat == "int":
                self.appendInt(self.val.cast(self.mpz_ptr), self.sortName)
            elif self.cat == "floating":
                self.appendFloat(self.val.cast(self.floating_ptr), self.sortName)
            elif self.cat == "stringbuffer":
                self.appendStringBuffer(self.val.cast(self.stringbuffer_ptr), self.sortName)
            self.var_names = {}
            self.used_var_name = set()
            return printKore(self.result, self.kompiled_dir)
        except:
             print(traceback.format_exc())
             raise

    def appendFloat(self, val, sort):
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
        self.result += "\\dv{" + sort + "}(\""
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
        self.result += "\")"


    def appendStringBuffer(self, val, sort):
        string = val.dereference()['contents'].dereference()['data'].string("iso-8859-1")
        self.result += "\\dv{" + sort + "}(\"" + string + "\")"

    def appendLimbs(self, size, ptr):
        accum = 0
        for i in range(size-1,-1,-1):
            accum <<= 64
            limb = int(ptr[i])
            accum |= limb
        self.result += str(accum)

    def appendInt(self, val, sort):
        self.result += "\\dv{" + sort + "}(\""
        size = int(val.dereference()['_mp_size'])
        if size == 0:
            self.result += "0"
        elif size < 0:
            self.result += "-"
            self.appendLimbs(-size, val.dereference()['_mp_d'])
        else:
            self.appendLimbs(size, val.dereference()['_mp_d'])
        self.result += "\")"

    def appendList(self, val, sort):
        length = val.dereference()['impl_']['size']
        if length == 0:
            self.result += "Lbl'Stop'List{}()"
            return
        i = 1
        for elem in ListIter(val):
            if i < length:
                self.result += "Lbl'Unds'List'Unds'{}("
            self.result += "LblListItem{}("
            self.append(elem.cast(self.block_ptr), False, "SortKItem{}")
            self.result += ")"
            if i < length:
                self.result += ","
            i += 1
        for i in range(length-1):
            self.result += ")"

    def appendMap(self, val, sort):
        length = val.dereference()['impl_']['size']
        if length == 0:
            self.result += "Lbl'Stop'Map{}()"
            return
        i = 1
        for entry in MapIter(val):
            key = entry['first']['elem']
            value = entry['second']['elem']
            if i < length:
                self.result += "Lbl'Unds'Map'Unds'{}("
            self.result += "Lbl'UndsPipe'-'-GT-Unds'{}("
            self.append(key.cast(self.block_ptr), False, "SortKItem{}")
            self.result += ","
            self.append(value.cast(self.block_ptr), False, "SortKItem{}")
            self.result += ")"
            if i < length:
                self.result += ","
            i += 1
        for i in range(length-1):
            self.result += ")"

    def appendSet(self, val, sort):
        length = val.dereference()['impl_']['size']
        if length == 0:
            self.result += "Lbl'Stop'Set{}()"
            return
        i = 1
        for entry in SetIter(val):
            elem = entry['elem']
            if i < length:
                self.result += "Lbl'Unds'Set'Unds'{}("
            self.result += "LblSetItem{}("
            self.append(elem.cast(self.block_ptr), False, "SortKItem{}")
            self.result += ")"
            if i < length:
                self.result += ","
            i += 1
        for i in range(length-1):
            self.result += ")"

    def append(self, subject, isVar, sort):
        address = int(subject.cast(self.long_int))
        if address == 0:
            self.result += "\\bottom{" + sort + "}()"
            return
        isConstant = address & 3
        if isConstant:
            tag = address >> 32
            if isConstant == 3:
                self.append(self.bound_variables[len(self.bound_variables)-1-tag], True, sort)
                return
            symbol = self.getSymbolNameForTag(tag).string()
            self.result += symbol + "()"
            return
        hdr = int(subject.dereference()['h']['hdr'])
        layout = hdr >> @LAYOUT_OFFSET@
        if not layout:
            string = subject.cast(self.string_ptr)
            length = hdr & @LENGTH_MASK@
            self.result += "\\dv{" + sort + "}(\""
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
            var = Variable(string)
            stdStr = var.stdStr
            if isVar and not var in self.var_names:
                suffix = ""
                while stdStr + suffix in self.used_var_names:
                    suffix = str(self.var_counter)
                    self.var_counter += 1
                stdStr = stdStr + suffix
                self.result += suffix
                self.used_var_names.add(stdStr)
                self.var_names[var] = suffix
            elif isVar:
                self.result += self.var_names[var]
            self.result += "\")"
            return
        tag = hdr & @TAG_MASK@
        isBinder = self.isSymbolABinder(tag)
        if isBinder:
            self.bound_variables.append((subject.cast(self.long_int) + 8).cast(self.block_ptr_ptr).dereference())
        symbol = self.getSymbolNameForTag(tag).string()
        if symbol[0:4] == "inj{":
            prefix = symbol[0:symbol.index(",")]
            symbol = prefix + ", " + sort + "}"
        self.result += symbol + "("
        layoutData = self.getLayoutData(layout)
        nargs = int(layoutData['nargs'])
        for i in range(nargs):
            argData = layoutData['args'] + i
            arg = subject.cast(self.long_int) + int(argData.dereference()['offset'])
            cat = argData.dereference()['cat']
            sort = gdb.lookup_global_symbol("sort_table").value()[tag][i].string("iso-8859-1")
            if cat == @MAP_LAYOUT@:
                self.appendMap(arg.cast(self.map_ptr), sort)
            elif cat == @LIST_LAYOUT@:
                self.appendList(arg.cast(self.list_ptr), sort)
            elif cat == @SET_LAYOUT@:
                self.appendSet(arg.cast(self.set_ptr), sort)
            elif cat == @SYMBOL_LAYOUT@:
                self.append(arg.cast(self.block_ptr_ptr).dereference(), False, sort)
            elif cat == @VARIABLE_LAYOUT@:
                self.append(arg.cast(self.block_ptr_ptr).dereference(), True, sort)
            elif cat == @INT_LAYOUT@:
                self.appendInt(arg.cast(self.mpz_ptr_ptr).dereference(), sort)
            elif cat == @FLOAT_LAYOUT@:
                self.appendFloat(arg.cast(self.floating_ptr_ptr).dereference(), sort)
            elif cat == @BOOL_LAYOUT@:
                string = "true" if arg.cast(self.bool_ptr).dereference() else "false"
                self.result += "\\dv{" + sort + "}(\"" + string + "\")"
            elif cat == @STRINGBUFFER_LAYOUT@:
                self.appendStringBuffer(arg.cast(self.stringbuffer_ptr_ptr).dereference(), sort)
            else:
                raise ValueError()
            if i != nargs - 1:
                self.result += ","
        if isBinder:
            self.bound_variables.pop()
        self.result += ")"

def kllvm_lookup_function(val):
    t = gdb.types.get_basic_type(val.type)
    if t.code != gdb.TYPE_CODE_PTR:
        if t.tag:
            kind = t.tag
        else:
            kind = t.name
        if kind[0:11] == 'immer::list':
            return termPrinter(val.address, "list", "SortList{}")
        elif kind[0:10] == 'immer::set':
            return termPrinter(val.address, "set", "SortSet{}")
        elif kind[0:10] == 'immer::map':
            return termPrinter(val.address, "map", "SortMap{}")
        return None
    s = t.target()
    if s.tag:
        kind = s.tag
        sort = val.type.name
    else:
        if val.type.name is None:
            return None
        kind = s.name
        sort = val.type.name + '{}'

    if kind == "block" or kind == "string":
        return termPrinter(val, "block", sort)
    elif kind == "list":
        return termPrinter(val, "list", sort)
    elif kind == "map":
        return termPrinter(val, "map", sort)
    elif kind == "set":
        return termPrinter(val, "set", sort)
    elif kind == "stringbuffer":
        return termPrinter(val, "stringbuffer", sort)
    elif kind == "__mpz_struct":
        return termPrinter(val, "int", sort)
    elif kind == "floating":
        return termPrinter(val, "floating", sort)
    return None

class KPrefix(gdb.Command):
    "Generic command for stepping through a K framework semantics."

    def __init__(self):
        super(KPrefix, self).__init__("k", gdb.COMMAND_NONE, gdb.COMPLETE_COMMAND, True)

prefix = KPrefix()

class KStart(gdb.Command):
    "Start the program but do not take any steps."

    def __init__(self):
        super(KStart, self).__init__("k start", gdb.COMMAND_RUNNING, gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        gdb.execute("start", from_tty)
        gdb.execute("advance definition.kore:step", from_tty)

start = KStart()

class KStep(gdb.Command):
    "Take a specified number of rewrite steps."

    def __init__(self, name, cat):
        super(KStep, self).__init__(name, cat, gdb.COMPLETE_NONE)

    def invoke(self, arg, from_tty):
        times = 1
        if arg != "":
            times = int(arg)
        if times > 0:
            bp = gdb.Breakpoint("definition.kore:step", internal=True, temporary=True)
            bp.ignore_count = times-1
            gdb.execute("c", from_tty)
        else:
            gdb.execute("step 0", from_tty)

step = KStep("k step", gdb.COMMAND_RUNNING)
step2 = KStep("k s", gdb.COMMAND_NONE)

class KMatch(gdb.Command):
    """Attempt to match a particular rule against a particular configuration.

Takes two arguments: The first is a top-level rule label.
The second is a term of sort GeneratedTopCell. For example, from the "step"
function, you could say something like "k match MODULE.LABEL subject"
to try to match the specified rule against the current configuration.

Does not actually take a step if matching succeeds.
    """

    def __init__(self):
        super(KMatch, self).__init__("k match", gdb.COMMAND_RUNNING, gdb.COMPLETE_SYMBOL)
        self.SUCCESS = 0
        self.FUNCTION = 1
        self.FAIL = 2

    def invoke(self, arg, from_tty):
        try:
            argv = gdb.string_to_argv(arg)
            gdb.lookup_global_symbol("resetMatchReason").value()()
            gdb.lookup_global_symbol(argv[0] + '.match').value()(gdb.parse_and_eval(argv[1]))
            entries = gdb.lookup_global_symbol("getMatchLog").value()()
            size = int(gdb.lookup_global_symbol("getMatchLogSize").value()())
            for i in range(size):
                entry = entries[i]
                if entry['kind'] == self.SUCCESS:
                    print('Match succeeds')
                elif entry['kind'] == self.FUNCTION:
                    print(entry['debugName'].string("iso-8859-1") + '(', end='')
                    function = gdb.lookup_global_symbol(entry['function'].string("iso-8859-1")).value().type
                    front = gdb.lookup_global_symbol("getMatchFnArgs").value()(entry.address)
                    conn = ""
                    for i in range(len(function.fields())):
                        arg = front[i]
                        argType = function.fields()[i].type
                        t = gdb.types.get_basic_type(argType)
                        if t.code != gdb.TYPE_CODE_PTR:
                            argStr = str(arg.cast(argType.pointer()).dereference())
                        else:
                            argStr = str(arg.cast(argType))
                        print(conn, end='')
                        print(argStr, end='')
                        conn = ', '

                    print(') => ', end='')
                    returnType = function.target()
                    t = gdb.types.get_basic_type(returnType)
                    if t.code != gdb.TYPE_CODE_PTR:
                        result = str(entry['result'].cast(returnType.pointer()).dereference())
                    else:
                        result = str(entry['result'].cast(returnType))
                    print(result)
                elif entry['kind'] == self.FAIL:
                    pattern = entry['pattern'].string("iso-8859-1")
                    sort = entry['sort'].string("iso-8859-1")
                    subject = str(entry['subject'].cast(gdb.lookup_type(sort)))
                    print("Subject:")
                    print(subject)
                    print("does not match pattern:")
                    print(printKore(pattern, getKompiledDir()))
        except:
             print(traceback.format_exc())
             raise

match = KMatch()
