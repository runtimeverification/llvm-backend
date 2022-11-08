# LLVM Backend Pattern Matching

The LLVM backend pattern matching algorithm is heavily based on the paper
"Compiling Pattern Matching to Good Decision Trees" by Luc Maranget [1]. We
will assume for the purposes of this documentation that the reader has read and
understands that paper in detail, and focus instead primarily on the
differences between that paper and the final algorithm used by the LLVM
backend. We will, however, exhaustively cover the preliminary concepts as they
differ somewhat in their final form from the original paper.

## Preliminary concepts

We expect the reader to be familiar with the basic notions of sorts,
constructors, patterns (linear and non-linear), actions, occurrences, pattern
matching matrices, specialization and default decomposition, decision trees,
and heuristics as described in the Maranget paper. We now cover these concepts
as they pertain to the algorithm.

### Sorts

For the purposes of the algorithm, we really only distinguish between four
*categories* of sorts: regular sorts, list sorts, map sorts, and set sorts.
We do, however, for convenience, describe the algorithm in terms of a
many-sorted sort structure where each sort has a set of constructors and no
subsorting exists, as opposed to simply describing it over each sort category.
Our algorithm does support dealing with subsorted K via matching modulo
equational axioms related to *injections*, which are a special constructor used
to inject subsorts into a many-sorted theory, but this document does not yet
describe the formal behavior of such matching.

### Constructors

For the purposes of the algorithm, we consider constructors to be a subset of
a pattern which contains certain specific information pertaining to the
top-level symbol of the pattern. Regular sorts have one constructor per
production, or, in the case of atomic built-in sorts like integers, floats, and
strings, an infinite family of constructors of arity 0. List sorts have an
infinite set of constructors `Len(i)` representing the family of all lists of
length `i`. Map and set sorts have the three constructors `Empty`, `Key(k)`,
and `Choice`, where `k` is a modified pattern explained in depth later on when
discussing the map/set matching algorithm.

### Patterns

Patterns have the following structure, described for convenience in BNF-style
notation:

```
syntax Pattern ::= Variable // variable patterns
                 | Constructor "(" [Pattern] ")"
                 | Pattern "as" Variable
                 | Or([Pattern])
                 | "List" [Pattern] // list patterns with no variable
                 | "List" [Pattern] ";" Variable ";" [Pattern] // list patterns with a variable
                 | "Set" [Pattern] // set patterns with no variable
                 | "Set" [Pattern] Variable // set patterns with a variable
                 | "Map" [(Pattern, Pattern)] // map patterns with no variable
                 | "Map" [(Pattern, Pattern)] Variable // map patterns with a variable
```

Variables come in two varieties: anonymous variables, which do not get
added to the substitution, and named variables, which do get added to the
substitution. Named variables, when matched, are bound to a particular
occurrence, which corresponds to a term at runtime. Anonymous variables match
any pattern, like a named variable, but are not bound to anything.

Note that one restriction not captured by the above grammar is that List, Set,
and Map patterns with a Variable must have at least one element Pattern.
Otherwise, they would simply be a variable pattern. The reason that `Set [] V`
is equal to `V` is that it can appear only in a context that expects a set
sort, which means that `List`, `Map`, or regular constructors cannot appear.
The same, obviously, is true respectively for lists and maps.

### Actions

An action is merely an integer uniquely identifying a particular rewrite rule.

### Occurrences

Occurrences are used to represent a term that appears during the matching
process at runtime, either because it is created during pattern matching, or
because it corresponds to a particular subterm of the term being matched on.

They have the following BNF-style syntax:

```
syntax Occurrence ::= [OccurrenceItem]
syntax OccurrenceItem ::= Int | "size" | "key" | "value"
                        | "rem" | "iter" | fresh(Int) | pat(Pattern)
```

Integers are used to refer to indices within lists and regular terms. `size` is
used to refer to the output of functions called to compute the size of a
collection. `key`, `value`, and `rem` are used in the context of map/set
matching in order to refer to the key and value of maps, the element of sets,
and the remainder after an element that has been matched has been removed from
the set. `iter` is used to refer to an iterator for a map or a set. `fresh` is
used to refer to the output of a side condition on a rule. `pat` is used to
identify *which* key or value a particular set or map occurrence is in relation
to.

### Matrices

A matrix represents an n-row by m-column grid of patterns, plus a *fringe* and
a *clause list*. The fringe consists of m occurrences and m sorts, one for each
column. We call these occurrences for convenience o_1, ..., o_m and the sorts
s_1, ..., s_m. What actual occurrence corresponds to o_1 depends on what
occurrence is in the first column of the matrix and will change as the matrix
transforms. The clause list consists of n clauses, one for each row. A clause C
consists of an action a, a substitution S (mapping variables to occurrences and
sorts), a boolean expression Cond (representing the side condition), and a list
R of type [(Occurrence, Sort, Int, Int)], used to compile list patterns. We
call R the set of `list ranges` bound by a particular row. This corresponds
to each variable `L` in a pattern of the form `List ps; L; qs`. We use this
information to ensure that the correct substitution for L is bound.
Conceptually, rows of a matrix are sorted into groups, which represent a set of
rows of equal priority. The notion of priority generalizes the concept from
Maranget's paper that rows at the top of the matrix apply first if they can,
and lower rows only apply if the upper rows cannot. Instead, any row in the
topmost group can apply if it matches, and rows in lower groups only apply if
no row in an upper group applies. This allows us to improve pattern matching
efficiency since K does not, for the most part, care what order rules are tried
in.

Matrices are first simplified to remove or-patterns by replacing each row
containing an or-pattern with a set of rows corresponding to all possible
permutations of the disjuncts of all or-patterns in the pattern.

### Decision Trees

A decision tree is a tree whose root represents the input of the matching
process and whose leaves identify which rewrite rule, if any, applies to the
term being matched. It has the following BNF-style syntax:

```
syntax DT ::= "Failure" // Fail in Maranget's paper
            | Success(Int, [(Occurrence, Sort)]) // Leaf in Maranget's paper
            | Switch(Occurrence, Sort, [(Constructor, DT)], DT?)
            | CheckNull(Occurrence, Sort, DT, DT)
            | Function(Occurrence, Function, [(Occurrence, Sort)], Sort, DT)
            | Pattern(Occurrence, Sort, Pattern, DT)
            | IterHasNext(Occurrence, Sort, DT, DT)
```

We will describe how each decision tree node is used later, but essentially,
Failure corresponds to the leaf case where no rule is matched, Success
corresponds to the leaf case where one particular rule is matched, Switch
corresponds to testing the constructor of a particular pattern, CheckNull
corresponds to testing whether a particular value is None, Function corresponds
to calling a particular function and binding the return value into the
substitution, `Pattern` corresponds to constructing a particular pattern and
binding it to an occurrence, and `IterHasNext` is very similar to `CheckNull`
except that it will be used to implement the behavior that backtracks to the
next element in a map or set in the case where we are iterating through a
collection testing whether each element will make matching succeed.

### Heuristics

The only heuristics regularly used by the LLVM backend are `q`, `b`, `a`, and
the `L` pseudo-heuristic. We will discuss in a later section how these
heuristics are adapted to the new features of the algorithm. We also discuss
how we extend the notion of heuristics to deal with the problem of choosing
in what order to decompose the keys of maps and sets.

## Compilation

Here we describe the compilation algorithm, the updated specialization and
default decompositions, and the adapted heuristics.

### Initial matrix

Starting from a set of rewrite rules of size n and their priorities, where each
rewrite rule has a left-hand side and a right-hand side of sort s, and
optionally a boolean condition Cond, we compute the initial matrix as follows:

P starts with one column and n rows. P_i1 = LHS[i]. o_1 = []. s_1 = s.
C_i = (a_i = i, S_i = empty, Cond_i = Cond[i], R_i = []).

The rows of P are sorted in increasing priority order, where each set of rows
with the same priority forms a group.

### Compile function

`Compile` is a function that takes a Matrix and returns a Decision Tree.

Described below is pseudocode for the function. This essentially corresponds
exactly to the `CC` function in Maranget's paper, except that rows are grouped
into groups with equal priority rather than totally ordered, and we explicitly
handle variables and side conditions. the `decomposeBestCol` function
essentially corresponds exactly to Maranget's algorithm in the case where the
sort of the chosen column is a regular sort.

```
def compile(Matrix m): DT
  if m has no rows:
    return Failure
  else:
    case bestRow(m) of
      None -> decomposeBestCol(m)
      BestRow(i) -> getLeaf(i, compile(m with row i removed))

def bestRow(Matrix m): Int
  if m has no columns:
    return the first row of m
  if there is a row in the topmost group of m where all columns are wildcard patterns:
    return that row
  else:
    return None

def getLeaf(Row i, DT dt): DT
  val newVars = empty
  for j = 1 to m:
    for each variable X in P_ij:
      newVars[X] = o_j
  val allVars = newVars U S_i
  val vars = sort the values of allVars by their keys alphabetically
  val atomicLeaf = Success(a_i, vars)
  if Cond_i is not None:
    val f = a function that evaluates Cond_i
    val sc = Function(fresh(a_i), f, vars, Bool, atomicLeaf)
  else:
    val sc = atomicLeaf
  return foldRanges(R_i, sc)

def decomposeBestCol(Matrix m): DT
  choose a column j that is best according to the heuristics
  if column j has a sort that is a regular sort:
    return Switch(o_j, s_j, compiledCases(m, j), compiledDefault(m, j))
  if column j has a sort that is a list sort:
    return decomposeList(m, j)
  if column j has a sort that is a map sort:
    return decomposeMap(m, j)
  if column j has a sort that is a set sort:
    return decomposeSet(m, j)

def compiledCases(Matrix m, Column j)
  return signature(s_j, column j of m).map(c -> (c, compile(specialize(m, j, c))))

def compiledDefault(Matrix m, Column j): DT?
  val m' = default(m, j)
  if m' is None:
    return None
  return compile(m')
```

Here is the definition, inductively, of a wildcard pattern:

* isWildcard(X) = true
* isWildcard(p as X) = isWildcard(p)
* isWildcard(_) = false otherwise

`foldRanges`, `decomposeList`, `decomposeMap`, and `decomposeSet` are described
as part of the sections on list, map, and set matching below.

### Signature function

`Signature` is a function that takes a column j of a matrix and a sort and
returns the set of constructors that must be considered when pattern matching
examines the term corresponding to o_j.

First we define `Signature` on patterns:

* signature(X : Variable) = []
* signature(c(p_1, ..., p_n)) = [c]
* signature(p as X) = signature(p)
* signature(List p_1, ..., p_n) = [Len(0), Len(1), ..., Len(n)]
* signature(List p_1, ..., p_n; L; q_1, ..., q_m) = [Len(0), Len(1), ..., Len(n+m)]
* signature(Set p_1, ..., p_n) = [Empty] if n == 0
* signature(Set p_1, ..., p_n) = [Key(p_1), ..., Key(p_n)] if n > 0
* signature(Set p_1, ..., p_n S) = [Key(p_1), ..., Key(p_n)]
* signature(Map p_1 |-> q_1, ..., p_n |-> q_n) = [Empty] if n == 0
* signature(Map p_1 |-> q_1, ..., p_n |-> q_n) = [Key(p_1), ..., Key(p_n)] if n > 0
* signature(Map p_1 |-> q_1, ..., p_n |-> q_n M) = [Key(p_1), ..., Key(p_n)]

We now define signature for columns in pseudocode:

```
def signature(Sort s, Column j): [Constructor]
  val sigma = union from i = 1 to n of signature(P_ij)
  if s is a map or set sort:
    if sigma contains Empty:
      return [Empty]
    else:
      return [bestKey(column j)]
  else:
    return sigma
```

`bestKey` will be defined later in the section for heuristics, but
essentially, it can be known to return either `Choice` or `Key(k)` for some
Key(k) in sigma.

### Specialize function

`Specialize` is a function that takes a column j of a matrix, a constructor c,
and a matrix m and returns a decomposed matrix representing the matrix after
having determined that the term at position o_j has constructor c.

`Specialize` operates by taking each row i of the matrix and either deleting it,
or transforming it into a new row i', depending on the structure of the pattern
P_ij.

We define specialize first on rows. We denote `a` as the arity of `c`. Note
that the cases for variables and regular constructors are essentially
identical to Maranget's definition of specialize except that we explicitly
define specialization to occur over any column instead of specifically the
first column. We also more thoroughly describe the behavior with respect to
variable bindings.

* specialize(c, i, X) = 
  [P_i1, ..., P_i(j-1), Y_1, ..., Y_a, P_i(j+1), ..., P_im] -> C
  where Y_1, ..., Y_a are fresh anonymous variables that do not appear in the
  substitution S, and C is the clause of row i except S_i is updated with the
  binding X |-> (o_j, s_j) if X is not an anonymous variable.
* specialize(c, i, c(p_1, ..., p_a)) =
  [P_i1, ..., P_i(j-1), p_1, ..., p_a, P_i(j+1), ..., P_im] -> C_i
* specialize(c, i, c'(p_1, ..., p_a')) where c != c' = None
* specialize(c, i, p as X) = specialize(c, i, p) except S_i is updated with the
  binding X |-> (o_j, s_j)

Specialization on list, set, and map patterns are described in their respective
sections.

We then also must define specialization on occurrence and sort vectors:

* specialize(c, [o_1, ..., o_m]) =
  [o_1, ..., o_(j-1), 1 : o_j, ..., a : o_j, o_(j+1), ..., o_m]
  if s_j is a normal sort
* specialize(c, [s_1, ..., s_m]) =
  [s_1, ..., s_(j-1), s_c1, ..., s_ca, s_(j+1), ..., s_m]
  if s_j is a normal sort
  where s_ci is the sort of the ith child of constructor c.

We then define specialize on matrices using the following pseudocode:

```
def specialize(Matrix m, Column j, Constructor c): Matrix
  for i = 1 to n:
    val newRow = specialize(c, i, P_ij)
    if newRow is None:
      delete row i
    else:
      replace row i with newRow
  replace fringe with specialized fringe
```

### Default function
`Default` is a function that takes a column j of a matrix and a matrix m and
returns a decomposed matrix representing the matrix after having determined
that the term at position o_j does not have any of the constructors in
signature(s_j, column j of m).

`Default` operates by taking each row i of the matrix and either deleting it,
or transforming it into a new row i', depending on the structure of the pattern
P_ij.

We define default first on rows. Once again, this corresponds more or less
exactly to Maranget's algorithm except for the inclusion of variable bindings,
as patterns, and the generalization to any column.


* default(i, X) = [P_i1, ..., P_i(j-1), P_i(j+1), ..., P_im] -> C
  where C is the clause of row i except S_i is updated with the binding
  X |-> (o_j, s_j)
* default(i, c(p_1, ..., p_a)) = None
* default(i, p as X) = default(i, p) except S_i is updated with the binding
  X |-> (o_j, s_j)

Default operations on maps, lists, and sets are described in their respective
sections.

We then also must define specialization on occurrence and sort vectors:

* default(i, [o_1, ..., o_m]) = [o_1, ..., o_(j-1), o_(j+1), ..., o_m]
* default(i, [s_1, ..., s_m]) = [s_1, ..., s_(j-1), s_(j+1), ..., s_m]

We then define default on matrices using the following pseudocode:

```
def default(Matrix m, Column j): Matrix
  if signature(s_j, column j of m) contains every constructor of sort s_j:
    return None
  if s_j is a normal sort, a map sort, or a set sort:
    for i = 1 to n:
      val newRow = default(i, P_ij)
      if newRow is None:
        delete row i
      else:
        replace row i with newRow
  else: //s_j is a list sort
    val (maxHead, maxTail) = maxListSize(column j)
    return specialize(m, j, Len(maxHead + maxTail))
```

`maxListSize` is defined in the section on list matching.

### Semantics of decision trees

A decision tree alone is not meaningful; it has meaning only insofar as it is
executable at runtime. We define the function `Evaluate` which takes a decision
tree and a concrete pattern (i.e., a pattern with no variables), and either
returns an action A and a substitution S mapping from occurrences to
concrete patterns, or the special value Fail.

Decision trees are not actually implemented this way in our implementation.
It is vastly more efficient to transform a decision tree into a native function
that takes a term as input in some internal representation and either raises
an exception (on failure) or calls a function which returns the right-hand side
of rule A substituted with S. However, we define it this way formally in order
to allow implementations to choose the details of exactly what machine code
to generate.

The function `evaluate` is defined statefully using an intermediate state
representing the substitution S which initially maps [] to the initial pattern,
and a stack of (DT, S) pairs that is initially empty. In order to
implement set/map matching, S is extended to allow us to map occurrences to an
iterator over a map or a set, or to None.

Note that not all the information in the decision tree is actually used by this
abstract algorithm. It is, however, kept in our description of decision trees
as it is necessary for generating native code.

We define evaluate in pseudocode:

```
type Subst = Occurrence |-> (Pattern \/ Iterator)
def evaluate(DT dt, Subst S, [(DT, Subst)] choices)
  case dt of
    Failure -> case choices of
                 [] -> return Fail
                 (dt', S') : tail -> evaluate(dt', S', tail)
    Success(A, _) -> return (A, S)
    Switch(o, s, cases, default) ->
      val c = the constructor of S[o]
      if c is in cases:
        for i = 1 to a: // arity of c
          S[i : o] = the ith child of S[o]
          return evaluate(decision tree for case c in cases, S, choices)
      else:
        return evaluate(default, S, choices)
    CheckNull(o, s, caseTrue, caseFalse) ->
      if S[o] is None:
        return evaluate(caseTrue, S, choices)
      else:
        return evaluate(caseFalse, S, choices)
    Function(o, f, args, s, dt) ->
      S[o] = f(args.map(S[_]))
      return evaluate(dt, S, choices)
    Pattern(o, s, pat, dt) ->
      S[o] = pat substituted with S
      return evaluate(dt, S, choices)
    IterHasNext(o, s, caseTrue, caseFalse) ->
      if S[o] is None:
        return evaluate(caseFalse, S, parent(dt) : choices)
      else:
        return evaluate(caseTrue, S, choices)
```

In the above, `parent` represents the parent node in the decision tree of a
particular node, which, in the case of `IterHasNext`, will correspond to a
Function node that will retry the matching process with the next element in the
collection.

### List Patterns

Having now described the algorithm for regular patterns, we must define various
pieces of the algorithm for list patterns. In particular, we must define
`foldRanges`, `decomposeList`, `maxListSize`, and `specialize` on list
patterns.

We start with `maxListSize` as it is quite simple. It is defined in pseudo-code
below:

```
def maxListSize(Column j): (Int, Int)
  val maxHead = 0
  val maxTail = 0
  for i = 1 to n:
    val p = P_ij
    while p is an as-pattern:
      p = child pattern of p
    case p of
      List ps ->
        maxHead = max(maxHead, length(ps))
      List ps; _; qs ->
        maxHead = max(maxHead, length(ps))
        maxTail = max(maxTail, length(qs))
      _ -> ()
  return (maxHead, maxTail)
```

Essentially, we return the longest head and longest tail of any list patterns
in column j.

Next, we define `specialize` on list patterns:

* specialize(Len(l), i, List p1, ..., p_l) =  
  [P_i1, ..., P_i(j-1), p1, ..., p_l, P_i(j+1), ..., P_im] -> C_i
* specialize(Len(l), i, List p1, ..., p_k) = None if k != l
* specialize(Len(l), i, List p1, ..., p_h; L; q_1, ..., q_t) =
  [P_i1, ..., P_i(j-1), p_1, ..., p_h, Y_1, ..., Y_(l-(h+t)), q_1, ..., q_t, P_i(j+1), ..., P_im] -> C
  if l > h + t
  where Y_1, ..., Y_(l-(h+t)) are fresh anonymous variables that do not appear
  in the substitution S, and C is the clause of row i except S_i is updated
  with the binding L |-> (l + 1 : o_j, s_j) and R_i is updated to add
  (l + 1 : o_j, s_j, h, t)
* specialize(Len(l), i, List p1, ..., p_h; L; q_1, ..., q_t) =
  [P_i1, ..., P_i(j-1), p_1, ..., p_h, q_1, ..., q_t, P_i(j+1), ..., P_im] -> C
  if l == h + t
  where C is the clause of row i except S_i is updated with the binding
  L |-> (l + 1: o_j, s_j) and R_i is updated to add (l + 1 : o_j, s_j, h, t)
* specialize(Len(l), i, List p1, ..., p_h; L; q_1, ..., q_t) = None
  if l < h + t

We did not define specialization on sort and occurrence vectors over list sorts
previously, so we also must define this now:

* specialize(Len(l), [o_1, ..., o_m]) =
  [o_1, ..., o_(j-1), 1 : o_j, ..., l : o_j, o_(j+1), ..., o_m]
  if s_j is a list sort
* specialize(Len(l), [s_1, ..., s_m]) =
  [s_1, ..., s_(j-1), s_1, ..., s_l, s_(j+1), ..., s_m]
  if s_j is a list sort
  where s_1, ..., s_l are all equal to the element sort of s_j

Now we have to define `decomposeList`. Roughly speaking, `decomposeList`
behaves similarly to the default Switch case except it switches over the
runtime length of the list and makes sure to add bindings for each list element
being matched on to the substitution. Note that in the case of the default, we
know that the list must be greater in length than any list pattern in the
column being decomposed, so we only must decompose maxHead elements from the
front of the list and maxTail elements from the end, since we can guarantee any
elements in between will never be referenced by any submatrix. This is the
reason why we treat the default operation as equivalent to specializing on
`maxHead + maxTail` elements.

```
def decomposeList(Matrix m, Column j): DT
  val newO = size : o_j
  val (maxHead, maxTail) = maxListSize(j)
  val listCases = compiledCases(m, j)
  for (Len(l), dt) in listCases:
     replace (Len(l), dt) with (Len(l), listGet(dt, l, o_j, s_j)) in listCases
  val listDefault = compiledDefault(m, j)
  listDefault = listGetEnd(listDefault, maxHead, maxTail, o_j, s_j)
  listDefault = listGet(listDefault, maxHead, o_j, s_j)
  val f = the function that returns the size of a list
  return Function(newO, f, [(o_j, s_j)], Int,
             Switch(newO, Int, listCases, listDefault))

def listGet(DT dt, Int len, Occurrence o, Sort s): DT
  if len == 0: 
    return dt
  val s' = the element sort of s
  val f = the function that returns the lenth element of a list
  return listGet(Function(len : o, f, [(o, s)], s', dt), len-1, o, s)

def listGetEnd(DT dt, Int maxHead, Int len, Occurrence o, Sort s): DT
  if len == 0:
    return dt
  val s' = the element sort of s
  val f = the function that returns the lenth element from the end of a list
  return listGetEnd(Function(len+maxHead, f, [(o, s)], s', dt), maxHead, len-1, o, s)
```

Finally, we must define `foldRanges`, which handles the R_i sections of a
clause C. This ensures that a binding for the L variable in list patterns with
variables exists by the time a rule is applied. The reason we defer this until
the end of the decision tree is that different rows may need to construct the
value of L differently even given the same runtime length of the list. Thus,
we cannot simply do it as part of the specialization process, or we will
disrupt sharing for the decision tree.

```
def foldRanges([(Occurrence, Sort, Int, Int)] R, DT dt): DT
  case R of
    [] -> dt
    (l : o, s, h, t) : tail ->
      val f = the function that removes h elements from the head of a list and t elements from its tail
      foldRanges(tail, Function(l : o, f, [(o, s)], s, dt))
```

### Canonicalized Patterns

Thus far we have not really given thought to how we define variables.

In most contexts, a variable is simply a name:

```
syntax Variable ::= String
```

However, there are a few contexts where using named variables is problematic.
In particular, in the `Constructor`, `Occurrence`, and `DT` sorts, we actually
need to be able to reference a variable by the occurrence that bound it. Thus,
in these contexts, we actually define a variable as follows:

```
syntax Variable ::= Occurrence
```

We thus have two notions of patterns: patterns whose variables are names, and
patterns whose variables are occurrences. As a result, we need some way of
converting between these representations.

As it happens, the clause list of a matrix provides this information in the
form of the substitution S. We thus say that to *canonicalize* a pattern p
with respect to row i of a Matrix m is to substitute all named variables in p
with their bindings in S_i. We say that a pattern is *bound* if all variables
in p have a binding in S_i.

### Map Patterns

K does not implement full AC matching on maps or sets. Instead, it attempts to
order the matching process in such a manner that wherever possible, the
substitution for all variables in the keys of map patterns are bound before
matching on the map itself. This allows us to, in the vast majority of cases,
compile matching on maps to map membership and lookup checks, which are vastly
more efficient than AC matching.

As a result, K has two types of matching on map patterns. The first, when the
key of the map element being matched on is bound, is called a *lookup*. In the
event that the pattern is not bound when matching occurs, we call this a
*map choice*. Essentially, the intuition behind a map choice is that we try
each element of the map in turn, proceeding as if it is the correct one, until
matching fails, at which point we backtrack to the next map element. In the
worst case, if matching a rule requires n map choices on a map of size m, then
the complexity of this algorithm is `O(m^n)`. This is, of course, worse than
the complexity of linear AC matching, which is `O(s*t^3)` where s is the size
of the pattern and t is the size of the term being matched [2]. However, in
practice the algorithm we will describe is quite fast for the vast majority of
rules written in real-world K semantics, and thus, we have not had much need to
try to optimize it further. This is generally because most matching either only
performs lookups, which can be performed in `O(log(n))` time, or it only
performs choices on a single varaible, which can be performed in `O(n)` time.
Cases where multiple choices are required usually indicate that the semantics
is badly optimized.

Note that, technically speaking, the algorithm does not care whether it
performs lookups or choices, and it does not care which keys are decomposed in
which order. Thus, we first describe the semantics of matching
nondeterministically with the assumption that *some* key will be chosen, and
describe the process of deciding /which/ key to choose in the section on
heuristics.

We now simply must fill in the missing cases in the implementation, namely,
`decomposeMap`, `specialize`, and `default`.

We start by defining `specialize` on map patterns:

* specialize(Empty, i, Map []) =
  [P_i1, ..., P_i(j-1), P_i(j+1), ..., P_im] -> C_i
* specialize(Empty, i, Map ps |-> qs) = None if length(ps) > 0
* specialize(Empty, i, Map ps |-> qs M) = None

* specialize(Key(k), i, Map ps |-> qs M) =
  [P_i1, ..., P_i(j-1), Y, Z, P_ij, P_i(j+1), ..., P_im] -> C_i
  if k is not in the canonicalized ps
  where Y and Z are fresh anonymous variables that do not appear in the
  substitution S
* specialize(Key(k), i, Map ps |-> qs M) =
  [P_i1, ..., P_i(j-1), q_k, Map ps' |-> qs' M, Y, P_i(j+1), ..., P_im] -> C_i
  if there exists a p_k in ps such that p_k == k
  where ps' |-> qs' is equal to ps |-> qs but without p_k |-> q_k and Y is a 
fresh anonymous variables that does not appear in the substitution S

* specialize(Key(k), i, Map ps |-> qs) =
  [P_i1, ..., P_i(j-1), Y, Z, P_ij, P_i(j+1), ..., P_im] -> C_i
  if there exists a p_k in ps such that p_k is bound and the canonicalized p_k
  unifies with k, but k is not in the canonicalized ps
* specialize(Key(k), i, Map ps |-> qs) =
  [P_i1, ..., P_i(j-1), q_k, Map ps' |-> qs', Y, P_i(j+1), ..., P_im] -> C_i
  if there exists a p_k in ps such that p_k is bound and p_k == k
  where ps' |-> qs' is equal to ps |-> qs but without p_k |-> q_k and Y is a 
fresh anonymous variables that does not appear in the substitution S
* specialize(Key(k), i, Map ps |-> qs) = None
  if for all p_k in ps, either p_k is not bound or the canonicalized p_k does
  not unify with k

* specialize(Choice, i, Map p |-> q : ps |-> qs M) =
  [P_i1, ..., P_i(j-1), p, q, Map ps |-> qs M, P_i(j+1), ..., P_im] -> C_i
* specialize(Choice, i, Map p |-> q : ps |-> qs) =
  [P_i1, ..., P_i(j-1), p, q, Map ps |-> qs, P_i(j+1), ..., P_im] -> C_i
  if row i is in the topmost group of m
* specialize(Choice, i, Map ps |-> qs) = None
  if row i is not in the topmost group of m
* specialize(Choice, i, Map []) = None

Note that in the above, if ps |-> qs = [], then Map ps |-> qs M is just a variable pattern M.

Intuitively, specializing on Empty creates no new columns. Specializing on
Key(k) creates 3 new columns corresponding to the value bound to key k, the
remaining map pattern after removing key k, and the original map itself. The
third category is required because we may test for membership of a key that
is only present in some rows of the matrix, and thus, for rows that may or may
not contain that key, we are doing no work and must still specialize on the
entire remaining map. Finally, specializing on Choice creates 3 new columns as
well, corresponding to the key patttern, the value pattern, and the remainder
of the map.

The reason why specializing on a choice requires the row to be in the topmost
group is that otherwise, we might fail to match the first key we try when
performing a choice, and then immediately try rules with lower priority, even
though we have not yet tried all the remaining keys of the map. Due to how map
matching works, we will always eventually try the remaining rules later on
in a separate part of the final decision tree.

Now we must define specialization on sort and occurrence vectors over map
sorts.

* specialize(Empty, [o_1, ..., o_m]) =
  [o_1, ..., o_(j-1), o_(j+1), ..., o_m]
* specialize(Key(k), [o_1, ..., o_m]) =
  [o_1, ..., o_(j-1), value : pat(k) : o_j, rem : pat(k) : o_j, o_j, o_(j+1), ..., o_m]
* specialize(Choice, [o_1, ..., o_m]) =
  [o_1, ..., o_(j-1), key : o_j, value : o_j, rem : o_j, o_(j+1), ..., o_m]

* specialize(Empty, [s_1, ..., s_m]) =
  [s_1, ..., s_(j-1), s_(j+1), ..., s_m]
* specialize(Key(k), [s_1, ..., s_m]) =
  [s_1, ..., s_(j-1), sv, s_j, s_j, s_(j+1), ..., s_m]
* specialize(Choice, [s_1, ..., s_m]) =
  [s_1, ..., s_(j-1), sk, sv, s_j, s_(j+1), ..., s_m]

where sk is the key sort of s_j and sv is the value sort of s_j.

We now define `default` on map columns:

* default(i, Map []) = None
  if Empty is in signature(s_j, j)
* default(i, Map ps |-> qs) =
  [P_i1, ..., P_im] -> C_i
  if length(ps) > 0 and Empty is in signature(s_j, j)
* default(i, Map ps |-> qs M) =
  [P_i1, ..., P_im] -> C_i
  if Empty is in signature(s_j, j)

* default(i, Map ps |-> qs) =
  [P_i1, ..., P_im] -> C_i
  if Key(k) is in signature(s_j, j) and the canonicalized ps does not contain k
* default(i, Map ps |-> qs M) =
  [P_i1, ..., P_im] -> C_i
  if Key(k) is in signature(s_j, j) and the canonicalized ps does not contain k
* default(i, Map ps |-> qs) =
  [P_i1, ..., P_im] -> C_i
  if Choice is in signature(s_j, j) and ps != [] and row i is not in the topmost group of m
* default(i, Map ps |-> qs M) =
  [P_i1, ..., P_im] -> C_i
  if Choice is in signature(s_j, j) and row i is not in the topmost group of m

* default(_, _) = None
  otherwise

Finally, we must define the `decomposeMap` function.

```
def decomposeMap(Matrix m, Column j): DT
  val newO = size : o_j
  val sk = key sort of s_j
  val sv = value sort of s_j
  if signature(s_j, j) contains Empty:
    val cases = [(0, compiledCases(m, j)[Empty])]
    val switch = Switch(newO, Int, cases, compiledDefault(m, j))
    val f = the function that returns the length of a map
    return Function(newO, f, [(o_j, s_j)], Int, switch)
  else if signature(s_j, j) contains Choice:
    val trueCase = compiledCases(m, j)[Choice]
    val falseCase = default(m, j)
    val f1' = the function that removes a key from a map
    val f2' = the function that looks up a key in a map
    val f3' = the function that returns the next element of an iterator, or None otherwise
    val f4' = the function that creates an iterator for a map
    val f1 = Function(rem : o_j, f1', [(o_j, s_j), (key : o_j, sk)], s_j, trueCase)
    val f2 = Function(value : o_j, f2', [(o_j, s_j), (key : o_j, sk)], sv, f1)
    val check = IterHasNext(key : o_j, sk, f2, falseCase)
    val f3 = Function(key : o_j, f3', [(iter : o_j, Iterator)], sk, check)
    return Function(iter : o_j, f4', [(o_j, s_j)], Iterator, f3)
    
  else:
    Key(k) is in signature(s_j, j)
    val cases = compiledCases(m, j)
    val check = CheckNull(value : pat(k) : o_j, sv,
        compiledDefault(m, j),
        cases[Key(k)])
    val f = the function that removes a key from a map
    val g = the function that returns the value of a key in a map if it exists and None otherwise
    val f1 = Function(rem : pat(k) : o_j, f, [(o_j, s_j), (newO, sk)], s_j, check)
    val f2 = Function(value : pat(k) : o_j, g, [(o_j, s_j), (newO, sk)], sv, f1)
    return Pattern(newO, sk, k, f2)
```

### Set Patterns

Intuitively, set patterns behave as if they were map patterns that mapped
the element sort of the set to the sort Unit, a sort with one constructor, ().
Practically, we can define set patterns explicitly in a similar fashion to map
patterns, except that set patterns do not have values and thus do not generate
any columns or decision tree nodes corresponding to the value sort of the map.
The rules are otherwise substantially identical, except for `decomposeSet`,
which we will now describe:

```
def decomposeSet(Matrix m, Column j): DT
  val newO = size : o_j
  val sk = element sort of s_j
  if signature(s_j, j) contains Empty:
    val cases = [(0, compiledCases(m, j)[Empty])]
    val switch = Switch(newO, Int, cases, compiledDefault(m, j))
    val f = the function that returns the length of a set
    return Function(newO, f, [(o_j, s_j)], Int, switch)
  else if signature(s_j, j) contains Choice:
    val trueCase = compiledCases(m, j)[Choice]
    val falseCase = default(m, j)
    val f1' = the function that removes an element from a set
    val f2' = the function that returns the next element of an iterator, or None otherwise
    val f3' = the function that creates an iterator for a set
    val f1 = Function(rem : o_j, f1', [(o_j, s_j), (key : o_j, sk)], s_j, trueCase)
    val check = IterHasNext(key : o_j, sk, f1, falseCase)
    val f2 = Function(key : o_j, f2', [(iter : o_j, Iterator)], sk, check)
    return Function(iter : o_j, f3', [(o_j, s_j)], Iterator, f3)
  else:
    Key(k) is in signature(s_j, j)
    val trueCase = compiledCases(m, j)[Key(k)]
    val falseCase = compiledDefault(m, j)
    val check = Switch(value : pat(k) : o_j, Bool,
        [(true, trueCase)], falseCase)
    val f = the function that removes an element from a set
    val g = the function that tests set membership
    val f1 = Function(rem : pat(k) : o_j, f, [(o_j, s_j), (newO, sk)], s_j, check)
    val f2 = Function(value : pat(k) : o_j, g, [(o_j, s_j), (newO, sk)], Bool, f1)
    return Pattern(newO, sk, k, f2)
```

### Heuristics

We have now fully described how to generate decision trees that successfully
match patterns of the `Pattern` sort described above. However, as is well
known, not all decision trees for a given problem are equal. In particular, the
algorithm makes no particular assumptions about which column is decomposed for
any given matrix, nor about which map key is chosen to decompose for any given
map column signature. Poor choices here can be catastrophic. As a result,
we need to redefine the q, b, a, and L heuristics used by K in light of the
differences between the Maranget paper and our own design, as well as defining
the `bestKey` function.

First, we start from the assumption we want to avoid set and map choices
whenever possible, as these are very expensive. Thus, we define the concept of
a *low-cost* or *high-cost* column. A column over list or regular sorts is
always low-cst. A column over map or set sorts is low-cost if all of its map
patterns are of the form Map [], or if there exists a key k in a map pattern in
the column such that k is bound.

We now ought to be able to define the `bestKey` function:

```
def bestKey(column j):
  val lowCostKeys = all keys in column j that are bound
  if lowCostKeys is empty: 
    return Choice
  else:
    for each key k in lowCostKeys:
      compute the score of column j according to key k
    return the key with the highest score
```

Now, we have to redefine each heuristic q, b, a, and L. To summarize, heuristic
q measures how many rows at the top of a column are not wildcard patterns, b
measures the number of constructors in the signature of the column, and a
measures the number of columns specializing on the column would create.
Finally, L simply chooses the leftmost column with the shortest occurrence.

Pseudo-heuristic L is essentially unchanged. The remaining heuristics differ in
that they are computed with respect to a particular key k in the case of map or
set columns. Essentially, we compute the signature for the purposes of the
heuristic as if `bestKey` always returned k. This allows the `bestKey` function
to compute the best key for each column, and pick the column with the best
score for its best key, with a minimum of repeated work.

One remaining difference is in the `q` heuristic, which unlike the others,
depends on the order of rows in the matrix. We generalize Maranget's notion of
q, which orders rows totally, to our grouping mechanism by iterating through
each group of rows, adding the number of rows with constructor patterns
anywhere in the group to the score, and stopping after that group unless all
rows in the group have constructor patterns. We consider any row a constructor
pattern iff its pattern is not a wildcard pattern in the sense defined in the
`Compile` section above.

Having defined this, it would seem we will have completely defined the scoring
mechanism: if any columns are low-cost, pick the low-cost column with the
highest score according to the combined heuristics as applied to the best key
of each column. Otherwise, pick the high-cost column with the higest score in
the same manner. This minimizes set/map choices while otherwise picking the
columns with the higest scores.

However, we actually soon realize there is a problem. A column may be high-cost
due to its lack of any keys that are bound, and yet it may still be better for
the final size of the shared decision tree to pick that column first, before
some other column which is currently low-cost. We realize that the case when
this occurs can actually be very neatly understood using the concept of
priority inversion. We can think of the columns as tasks with priorities equal
to their scores, and dependencies such that each high-cost column depends on
those columns which contain variables present in the keys of the first column
in the same row (ie, decomposing the second column and the columns created by
doing so repeatedly would eventually add the bindings to S which make the first
column low-cost). It then becomes natural to think of the case when a higher
priority column depends on a lower priority column as a priority inversion. The
natural solution to priority inversion problems becomes to assign the effective
priority of the low priority column equal to the highest priority of all the
columns that depend on it. When we do this, we see that the decision tree
algorithm will naturally chose columns in the correct order in order to
minimize decision tree size.

Provided below is a simple pseudo-code which can be used to compute the effective score of a column:

```
def effectiveScore(Column j, Matrix m):
  bestInvalid = None
  for column c in m:
    if c is low-cost:
      continue
    if column c needs column j:
      if bestInvalid is None:
        bestInvalid = c
      else if c's score is higher than bestInvalid's score:
        bestInvalid = c
  if bestInvalid is None:
    return j's score
  else:
    return bestInvalid's score
```

A column j needs another column k if there exists a row i in m and a variable X such that X is mentioned in P_ik and X is mentioned in the key of a map or set pattern in P_ij.

## Iterated Optimization

One particular property of rewriting which is distinct from other forms of
pattern matching is that rewriting consists of the process of repeatedly
matching and transforming a term. K in particular also has the property that
its rewrites, with the exception of function rules, are *top-level rewrites*;
in other words, pattern matching on K configurations with rewrite rules happens
only at the top level. As a result, it becomes natural to think of rewriting
as the process of taking a term, matching the term against the left-hand side
of a rule, then applying the substitution to the right-hand side and repeating
the process.

It turns out this duplicates a lot of effort; when a configuration is primarily
made up of constructors, it is possible to know statically that certain rules
cannot apply immediately after other rules. For example, consider the following
simple rewrite system:

```
syntax Foo ::= "foo" | "bar" | "baz" | "end"
rule foo => bar
rule bar => baz
rule bar => end
rule baz => end
```

It is clear to the viewer that if the configuration starts as "foo", only the
first rule can apply, after which the second and third rule can apply, but not
the fourth. Further, after the second rule applies, only the fourth rule can
apply, and after the third or fourth rule applies, no other rules can apply.

It turns out that the matrices used to compile decision trees can not only be
efficiently used to compute the set of rules that can apply immediately
following another rule, but we can actually take advantage of the information
on the right-hand side of each rule to generate a *specialized decision tree*
that can be used to match the term after that rule applies, without any changes
to the transition system of the rewriter. These specialized decision trees
have the property that not only do they only need to consider rules that can
apply immediately following the previous rule, they do not need to redo any
pattern matching on the parts of the terms which were constructors in the
right-hand side of the previous rule. This significantly brings down the amount
of pattern matching necessary to implement an interpreter, at the cost of a
rather significant increase in code size. Overall, measurements show that this
can have a sizeable impact on the performance of the resulting interpeter in
cases where the interpreter was previously bounded in performance by the
performance of pattern matching.

We call this optimization the "iterated pattern matching optimization" insofar
as it relates to properties of the problem of repeatedly matching terms against
the same set of patterns multiple times in immediate succession. 

The algorithm is relatively simple: We start from the initial matrix containing
all the rules in the semantics. Then, for each rule, we *specialize* the matrix
by the right-hand side of that rule. The intuition behind this operation is
that we decompose the matrix using the `specialize` operation based on the
information present in that right-hand side. Once we are done specializing, we
are left with a specialized matrix, and a list of patterns from the right-hand
side which could not be specialized, which we will call *residual patterns*,
or residuals. There will be exactly one residual for each column remaining in
the specialized matrix. We then compile the resulting matrix, and, together
with the mapping between occurrences and residuals, this information is used
to perform iterated pattern matching.

There are several subtleties besides the specific implementation of the
`specializeBy` function, however. One in particular is that the matrix after
being specialized generally has multiple columns, corresponding to multiple
inputs to the pattern matching function. We thus must define how those inputs
are created from the previous term. Further, if any of the constructors
matched statically by the pattern match compiler have variables that are bound
as part of the `specializeBy` function, these variables will exist in the
substitution for that row in the matrix, but they will not exist in the
substitution generated by the `evaluate` function.

Thus, in order to fully define this optimization, we must address all three
concerns: how to generate the different inputs to the `evaluate` function, how
to compile the matrix to a decision tree which generates all the relevant
bindings for statically-matched terms, and how the `specializeBy` function
operates.

We begin with the `evaluate` function.

Unlike before, where evaluation took as input a term and a decision tree,
it now takes a decision tree and an initial substitution. The initial
substitution contains one binding per entry in the residual map. Bear in mind
the keys in the residual map are occurrences and the values are terms from the
right-hand side of a rule, which may contain variables bound by the left-hand
side of that rule, and may also contain things like functions and such which
are not traditionally thought of as patterns.

We can construct the initial substitution of the `evaluate` function quite
easily, as it turns out. Simply substitute the variables from the left-hand
side of the previous rule into the residuals that were part of that rule's
right-hand side, evaluating the result according to the same semantics used
for the right-hand side of rules in your language. Then bind them to the same
occurrences they were bound to in the residual map itself, and pass that map to
the `evaluate` function. As before, we start with an empty stack, and the
algorithm proceeds unchanged from that point.

However, we still need to address the issue of bindings for variables that
were bound as part of the execution of the `specializeBy` function. It turns
out the solution is to add a new field to each row in the matrix recording
the variables that were bound by the `specializeBy` function. Then, in the
`getLeaf` function, we modify the return value `dt` to return something like
`MakePattern(o, s, p, dt)` where `o` and `s` are the occurrence and sort
for the variable that was bound, and `p` is the pattern that it was bound to.

Finally, we must define the `specializeBy` function. We define it quite
simply in pseudo-code:

```
def specializeBy(Matrix m, [Term] rhs): (Matrix, [Term])
  if m has no rows:
    return (m, rhs)
  if every term t in rhs is a wildcard pattern or is not a pattern:
    return (m, rhs)
  pick the best column j of m such that rhs[j] is a non-wildcard pattern
  val c = the constructor of rhs[j]
  return specializeBy(specialize(m, j, c), specialize(rhs, j, c))
```

We generalize the `specialize` function from matrices to vectors of patterns
by treating the vector of patterns as a single-rowed matrix with the same
fringe as m and an arbitrary clause whose result is discarded.

We have not tested the algorithm against list, map, or set patterns, but it
ought to function on such examples according to the same essential principles.
a list pattern with a variable would be treated as a wildcard pattern, since
its length at runtime would be unknown, but other collection patterns would be
treated as constructors. The constructor of a list pattern with no variable is
its length, the constructor of the empty map or set would be Empty, and the
constructor of any other map or set pattern would be some arbitrarily chosen
Key(k). Care would have to be taken that if the signature of a column in m was
`[Empty]`, but the signature of the corresponding pattern in `rhs` was
`Key(k)`, that the matrix first be transformed via the `default` function to 
remove rows containing empty collections.

One final thing to note is that by itself, the above algorithm generally
increases the code size of the interpreter binary quite substantially. A 
large part of this is that some rules, when you call `specializeBy` on them,
return a matrix that is not much smaller than the original matrix. This leads
to the property that instead of a linear size in the total decision trees,
the size is closer to `O(n*m)` where n is the size of the original decision tree
and m is the number of rules.

The solution we applied to deal with this is to set a threshold heuristic as
a number between 0 and 1. We then compare the number of rows in the specialized
matrix to the number of rows in the original matrix, as a fraction. If the
fraction is larger than the threshold, we say that the matrix does not
specialize well for this rule, and we simply discard that matrix and the
resulting decision tree and fall back on constructing the original RHS of the
rule and using the original decision tree at runtime. We have found that a
threshold of 1/2 works quite well for us, but this can be increased in order
to improve the execution speed somewhat, or decreased to decrease the code
size somewhat. Of course, in either direction you will eventually see
diminishing returns.

## Citations

[1] Maranget, Luc.
    "Compiling pattern matching to good decision trees."
    Proceedings of the 2008 ACM SIGPLAN workshop on ML. 2008.

[2] Benanav, Dan, Deepak Kapur, and Paliath Narendran.
    "Complexity of matching problems."
    Journal of symbolic computation 3.1-2 (1987): 203-216.
