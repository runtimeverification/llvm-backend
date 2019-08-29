import gdb.printing
import traceback
import time

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
codes["Bash"] = "\\"
codes["Xor-"] = "^"
codes["Unds"] = "_"
codes["BQuo"] = "`"
codes["LBra"] = "{"
codes["Pipe"] = "|"
codes["RBra"] = "}"
codes["Tild"] = "~"

class MapIter:
    def __init__(self, val):
        self.it = gdb.lookup_global_symbol("datastructures::hook_map::map_iterator_dbg").value()(val)

    def __iter__(self):
        return self

    def __next__(self):
        val = gdb.lookup_global_symbol("datastructures::hook_map::map_iterator_next_dbg").value()(self.it)
        if val:
            return val
        raise StopIteration


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

class blockPrinter:
    """Print a block * object."""

    def __init__(self, val):
        self.val = val
        self.long_int = gdb.lookup_type("long int")
        self.string_ptr = gdb.lookup_type("string").pointer()
        self.block_ptr = gdb.lookup_type("block").pointer()
        self.block_ptr_ptr = gdb.lookup_type("block").pointer().pointer()
        self.mpz_ptr_ptr = gdb.lookup_type("__mpz_struct").pointer().pointer()
        self.map_ptr = gdb.lookup_type("map").pointer()

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
            self.append(self.val, False)
            self.var_names = {}
            self.used_var_name = set()
            return self.result
        except:
             print(traceback.format_exc())
             raise

    def appendLimbs(self, size, ptr):
        accum = 0
        for i in range(size):
            accum *= 1 << 64
            limb = int(ptr[i])
            accum += limb
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

    def appendMap(self, val):
        length = val.dereference()['a']
        if length == 0:
            self.result += "`.Map`(.KList)"
            return
        i = 1
        for key in MapIter(val):
            value = gdb.lookup_global_symbol("hook_MAP_lookup").value()(val, key)
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
                c = chr(int(string.dereference()['data'][i]))
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
                    self.result += "{:02x}".format(c)
            stdStr = string.dereference()['data'].string()
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
                self.appendFloat(arg.cast(self.mpfr_ptr_ptr).dereference())
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
    if t.code == gdb.TYPE_CODE_PTR and t.target().tag and t.target().tag == "block":
        return blockPrinter(val)
    return None
