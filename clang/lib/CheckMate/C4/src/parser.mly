%{

  open Hack
    
%}
%token PTR
%token TSTRUCT
%token <string> TSTRUCT_str "Tstruct"
%token TAINTED
%token CALLBACK
%token MIRROR
%token TPTR
%token <int * int> COLONBOUNDS
%token <int * int> COLONITYPE
%token <string> ANY
%token <int * int> PRAGMA
%token <string> ID
%token <string> PID
%token EOF
%token LANGLE
%token RANGLE
%token LPAREN
%token RPAREN
%token FORANY
%token COLON
%token COMMA
%token GIJOE
%token CHECKED
%token DYNCHECK
%token ASSUME_CAST
(*
        stdlib_tainted.h
*)

%token TATOF
%token TATOI
%token TATOL
%token TATOLL
%token TSTRTOD
%token TSTRTOF
%token TSTRTOLD
%token TSTRTOL
%token TSTRTOLL
%token TSTRTOUL
%token TSTRTOULL
%token TALIGNEDALLOC
%token TFREE
%token TATEXIT
%token TATQUICKEXIT
%token TSYSTEM
%token TBSEARCH
%token TGETENV
%token TQSORT
%token TMBLEN
%token TMBTOWC
%token TMBSTOWCS
%token TWCSTOMBS
%token TMALLOC
%token TREALLOC
%token TSPRINTFCHKCBX
%token TSPRINTF
(*
        _builtin_common_tainted.cpp
*)
%token TBUILTINOBJECTSIZE
(*
        _builtin_stdio_tainted.h
*)
%token TBUILTINSPRINTFCHK
%token TSNPRINTFCHK
%token TBUILTINSNPRINTFCHK
%token TBUILTINVSPRINTFCHK
%token TVSNNPRINTFCHK
%token TBUILTINVSNPRINTFCHK
%token TFRINTFCHK
%token TBUILTINFPRINTFCHK
%token TPRINTFCHK
%token TBUILTINPRINTFCHK
%token TVSPRINTFCHK
%token TVFPRINTFCHK
%token TBUILTINVFPRINTFCHK
%token TBULTINVPRINTFCHK
%token TPRINTF
%token TFREEXP
%token TFREEXPF
%token TFREEXPL
%token TMODF
%token TMODFF
%token TMODFL
%token TREMQUOF
%token TREMQUOL
%token TNAN
%token TNANF
%token TNANL
%token TISNAN
%token TISINF
%token TERRNO
%token TERRNOLOCATION
%token TFREAD
%token TFCLOSE
%token TFOPEN
%token TFSEEK
%token TFTELL
%token TREWIND
%token TFERROR
%token TMEMCPY
%token TMEMMOVE
%token TMEMSET
%token TSTRCPY
%token TSTRNCPY
%token TSTRCAT
%token TSTRNCAT
%token TMEMCMP
%token TSTRCMP
%token TSTRCOLL
%token TSTRNCMP
%token TSTRXFRM
%token TMEMCHR
%token TSTRCHR
%token TSTRCSPN
%token TSTRPBRK
%token TSTRRCHR
%token TSTRSPN
%token TSTRTOK
%token TSTRERROR
%token TSTRLEN
%token TSTRDUP
%token TSTRSTR
%token TREMQUO
%start <(int*int*string) list> main

%%

main:
| p = checkedptr m = main { ($startpos.pos_cnum , $endpos(p).pos_cnum, p)::m }
| p = cast m = main { ($startpos.pos_cnum , $endpos(p).pos_cnum, p)::m }
| p = annot m = main { p::m }
| LPAREN m = main { m }
| RPAREN m = main { m }
| LANGLE m = main { m }
| RANGLE m = main { m }
| COLON m = main { m }
| PID popt = instvar? m = main { match popt with Some p -> p::m | None -> m }
| ID m = main { m }
| COMMA m = main { m }
| ANY m = main { m }
| EOF { [] }

cast: 
  | ASSUME_CAST LANGLE  i = insideitype RANGLE LPAREN e = expr COMMA insidebounds* RPAREN { "("^i^")"^e } 
  | ASSUME_CAST LANGLE  i = insideitype RANGLE LPAREN e = expr RPAREN {  "("^i^")"^e }

expr:
| LPAREN e = exprcomma* RPAREN {  (String.concat "" ("("::e))^")" }
| c = ANY s = expr { String.concat "" [c; s]}
| LANGLE s = expr { String.concat "" ["<"; s]}
| RANGLE s = expr { String.concat "" [">"; s]}
| COLON s = expr { String.concat "" [":"; s]}
/* This is not properly capturing whitespace: it assumes there's a space between tokens, but that's not necessarily so. Need to fix lexer.  */
| c = ID s = expr { String.concat " " [c; s]}
| LANGLE { "<" }
| RANGLE { ">" }
| COLON { ":" }
| c = ANY { c }
| c = ID { c }

tstruct:
| TPTR LANGLE TSTRUCT s = tstruct RANGLE {String.concat "" ["*"; s ]}

tmalloc:
| TMALLOC LANGLE tmalloc RANGLE {  String.concat "" [""; ""] }

tfree:
| TFREE LANGLE tfree RANGLE {  String.concat "" [""; ""] }

trealloc:
| TREALLOC LANGLE trealloc RANGLE {  String.concat "" [""; ""] }

tmemcpy:
| TMEMCPY LANGLE tmemcpy RANGLE {  String.concat "" [""; ""] }

tfread:
| TFREAD  LANGLE tfread RANGLE {  String.concat "" [""; ""] }

exprcomma:
| LPAREN e = exprcomma* RPAREN { (String.concat "" ("("::e))^")" }
| c = ANY s = exprcomma { String.concat "" [c; s]}
| LANGLE s = exprcomma { String.concat "" ["<"; s]}
| RANGLE s = exprcomma { String.concat "" [">"; s]}
| COMMA s = exprcomma { String.concat "" [","; s]}
| COLON s = exprcomma { String.concat "" [":"; s]}
/* This is not properly capturing whitespace: it assumes there's a space between tokens, but that's not necessarily so. Need to fix lexer.  */
| c = ID s = exprcomma { String.concat " " [c; s]}
| LANGLE { "<" }
| RANGLE { ">" }
| COLON { ":" }
| COMMA { "," }
| c = ANY { c }
| c = ID { c }
        
instvar:
| LANGLE insideitype RANGLE  { ($startpos.pos_cnum, $endpos.pos_cnum, "") }

annot:
/* add INCLUDE here; remove _checked, drop stdchecked.h (and note it in lexer) */
| CHECKED { ($startpos.pos_cnum, $endpos.pos_cnum, "") }
| TAINTED { ($startpos.pos_cnum, $endpos.pos_cnum, "")}
| CALLBACK { ($startpos.pos_cnum, $endpos.pos_cnum, "")}
| TFCLOSE { ($startpos.pos_cnum, $endpos.pos_cnum, "fclose")}
| TFREAD { ($startpos.pos_cnum, $endpos.pos_cnum, "fread")}
| TFOPEN { ($startpos.pos_cnum, $endpos.pos_cnum, "fopen")}
| TFSEEK { ($startpos.pos_cnum, $endpos.pos_cnum, "fseek")}
| TFTELL { ($startpos.pos_cnum, $endpos.pos_cnum, "ftell")}
| TREWIND { ($startpos.pos_cnum, $endpos.pos_cnum, "rewind")}
| TFERROR { ($startpos.pos_cnum, $endpos.pos_cnum, "ferror")}
| MIRROR { ($startpos.pos_cnum, $endpos.pos_cnum, "")}
| TMALLOC LANGLE tmalloc RANGLE { ($startpos.pos_cnum, $endpos.pos_cnum, "malloc")}
| TMEMCPY { ($startpos.pos_cnum, $endpos.pos_cnum, "memcpy") }
| TMEMCPY LANGLE tmemcpy RANGLE { ($startpos.pos_cnum, $endpos.pos_cnum, "memcpy") }
| TSTRUCT { ($startpos.pos_cnum, $endpos.pos_cnum, "struct") }
| TATOF { ($startpos.pos_cnum, $endpos.pos_cnum, "atof") }
| TATOI { ($startpos.pos_cnum, $endpos.pos_cnum, "atoi") }
| TATOLL { ($startpos.pos_cnum, $endpos.pos_cnum, "atoll") }
| TSTRTOD { ($startpos.pos_cnum, $endpos.pos_cnum, "strtod") }
| TSTRTOF { ($startpos.pos_cnum, $endpos.pos_cnum, "strtof") }
| TSTRTOLD { ($startpos.pos_cnum, $endpos.pos_cnum, "strtold") }
| TSTRTOL { ($startpos.pos_cnum, $endpos.pos_cnum, "strtol") }
| TSTRTOLL { ($startpos.pos_cnum, $endpos.pos_cnum, "strtoll") }
| TSTRTOUL { ($startpos.pos_cnum, $endpos.pos_cnum, "strtoul") }
| TSTRTOULL { ($startpos.pos_cnum, $endpos.pos_cnum, "strtoull") }
| TALIGNEDALLOC { ($startpos.pos_cnum, $endpos.pos_cnum, "aligned_alloc") }
| TFREE LANGLE tfree RANGLE{ ($startpos.pos_cnum, $endpos.pos_cnum, "free") }
| TREALLOC LANGLE trealloc RANGLE { ($startpos.pos_cnum, $endpos.pos_cnum, "realloc") }
| TGETENV { ($startpos.pos_cnum, $endpos.pos_cnum, "getenv") }
| TATEXIT { ($startpos.pos_cnum, $endpos.pos_cnum, "atexit") }
| TATQUICKEXIT { ($startpos.pos_cnum, $endpos.pos_cnum, "atquick_exit") }
| TSYSTEM { ($startpos.pos_cnum, $endpos.pos_cnum, "system") }
| TBSEARCH { ($startpos.pos_cnum, $endpos.pos_cnum, "bsearch") }
| TQSORT { ($startpos.pos_cnum, $endpos.pos_cnum, "qsort") }
| TMBLEN { ($startpos.pos_cnum, $endpos.pos_cnum, "mblen") }
| TMBTOWC { ($startpos.pos_cnum, $endpos.pos_cnum, "mbtowc") }
| TMBSTOWCS { ($startpos.pos_cnum, $endpos.pos_cnum, "mbstowcs") }
| TPRINTF { ($startpos.pos_cnum, $endpos.pos_cnum, "printf") }
| TFREEXP { ($startpos.pos_cnum, $endpos.pos_cnum, "frexp") }
| TFREEXPF { ($startpos.pos_cnum, $endpos.pos_cnum, "frexpf") }
| TFREEXPL { ($startpos.pos_cnum, $endpos.pos_cnum, "frexpl") }
| TMODF { ($startpos.pos_cnum, $endpos.pos_cnum, "modf") }
| TMODFF { ($startpos.pos_cnum, $endpos.pos_cnum, "modff") }
| TMODFL { ($startpos.pos_cnum, $endpos.pos_cnum, "modfl") }
| TREMQUO { ($startpos.pos_cnum, $endpos.pos_cnum, "remquo") }
| TREMQUOF { ($startpos.pos_cnum, $endpos.pos_cnum, "remquof") }
| TREMQUOL { ($startpos.pos_cnum, $endpos.pos_cnum, "remquol") }
| TNAN { ($startpos.pos_cnum, $endpos.pos_cnum, "nan") }
| TNANF { ($startpos.pos_cnum, $endpos.pos_cnum, "nanf") }
| TNANL { ($startpos.pos_cnum, $endpos.pos_cnum, "nanl") }
| TISNAN { ($startpos.pos_cnum, $endpos.pos_cnum, "isnan") }
| TISINF { ($startpos.pos_cnum, $endpos.pos_cnum, "isinf") }
| CALLBACK { ($startpos.pos_cnum, $endpos.pos_cnum, "") }
| TERRNO { ($startpos.pos_cnum, $endpos.pos_cnum, "errno") }
| TERRNOLOCATION { ($startpos.pos_cnum, $endpos.pos_cnum, "__errno_location") }
| TMEMMOVE { ($startpos.pos_cnum, $endpos.pos_cnum, "memmove") }
| TMEMSET { ($startpos.pos_cnum, $endpos.pos_cnum, "memset") }
| TSTRCPY { ($startpos.pos_cnum, $endpos.pos_cnum, "strcpy") }
| TSTRNCPY { ($startpos.pos_cnum, $endpos.pos_cnum, "strncpy") }
| TSTRCAT { ($startpos.pos_cnum, $endpos.pos_cnum, "strcat") }
| TSTRNCAT { ($startpos.pos_cnum, $endpos.pos_cnum, "strncat") }
| TMEMCMP { ($startpos.pos_cnum, $endpos.pos_cnum, "memcmp") }
| TSTRCMP { ($startpos.pos_cnum, $endpos.pos_cnum, "strcmp") }
| TSTRCOLL { ($startpos.pos_cnum, $endpos.pos_cnum, "strcoll") }
| TSTRNCMP { ($startpos.pos_cnum, $endpos.pos_cnum, "strncmp") }
| TSTRXFRM { ($startpos.pos_cnum, $endpos.pos_cnum, "strxfrm") }
| TMEMCHR { ($startpos.pos_cnum, $endpos.pos_cnum, "memchr") }
| TSTRCHR { ($startpos.pos_cnum, $endpos.pos_cnum, "strchr") }
| TSTRCSPN { ($startpos.pos_cnum, $endpos.pos_cnum, "strcspn") }
| TSTRPBRK { ($startpos.pos_cnum, $endpos.pos_cnum, "strpbrk") }
| TSTRRCHR { ($startpos.pos_cnum, $endpos.pos_cnum, "strrchr") }
| TSTRSPN { ($startpos.pos_cnum, $endpos.pos_cnum, "strspn") }
| TSTRSTR { ($startpos.pos_cnum, $endpos.pos_cnum, "strstr") }
| TSTRTOK { ($startpos.pos_cnum, $endpos.pos_cnum, "strtok") }
| TSTRERROR { ($startpos.pos_cnum, $endpos.pos_cnum, "strerror") }
| TSTRLEN { ($startpos.pos_cnum, $endpos.pos_cnum, "strlen") }
| TSTRDUP { ($startpos.pos_cnum, $endpos.pos_cnum, "strdup") }
| TSPRINTFCHKCBX {($startpos.pos_cnum, $endpos.pos_cnum, "__sprintf_chkcbx")}
| TSPRINTF {($startpos.pos_cnum, $endpos.pos_cnum, "sprintf")}
| TPRINTF {($startpos.pos_cnum, $endpos.pos_cnum, "printf")}
| p = PRAGMA { let (s,e) = p in (s, e, "") }
| DYNCHECK LPAREN insidebounds* RPAREN { ($startpos.pos_cnum, $endpos.pos_cnum, "") }
| FORANY LPAREN t = ID RPAREN { note_tyvar t; ($startpos.pos_cnum, $endpos.pos_cnum, "") }
| p = bounds { let (s,_) = p in (s, $endpos.pos_cnum, "") }
| p = itype fakebounds* { let (s,_) = p in (s, $endpos.pos_cnum, "") }

bounds:
| p = COLONBOUNDS LPAREN insidebounds* RPAREN { p }

fakebounds: /* I am assuming ID will be count, bounds, or byte_count */
| ID LPAREN insidebounds+ RPAREN { None }

insidebounds:
| TSTRUCT insidebounds {None}
| LPAREN insidebounds+ RPAREN { None }
| checkedptr { None }
| LANGLE { None }
| RANGLE { None }
| COLON { None }
| COMMA { None }
| ID { None }
| ANY { None }

itype:
| p = COLONITYPE LPAREN insideitype RPAREN { p }

insideitype:
| c = ANY s = insideitype { String.concat "" [c; s]}
/* This is not properly capturing whitespace: it assumes there's a space between tokens, but that's not necessarily so. Need to fix lexer.  */
| c = ID s = insideitype { String.concat " " [c; s]}
| c = checkedptr { c }
| c = ID { c }
| c = ANY { c }

checkedptr:
| PTR LANGLE p = checkedptr RANGLE { String.concat "" [p; " *"] }
| PTR LANGLE s = insideptr RANGLE { String.concat "" [s; " *"]}
| PTR LANGLE fp = fpointer RANGLE name = id_or_pid 
  { let (ret,params) = fp in String.concat "" [ret; "(*"; name; ")"; params] }
(* "anonymous" function pointers *)
| PTR LANGLE fp = fpointer RANGLE 
  { let (ret,params) = fp in String.concat "" [ret; "(*"; ")"; params] }
| TPTR LANGLE p = checkedptr RANGLE { String.concat "" [p; " *"] }
| TMALLOC LANGLE insidebounds RANGLE {  String.concat "malloc" [""; ""]}
| TFREE LANGLE insidebounds RANGLE {  String.concat "" ["free"; ""]}
| TREALLOC LANGLE insidebounds RANGLE {  String.concat "realloc" [""; ""]}
| TMEMCPY LANGLE insidebounds RANGLE {  String.concat "memcpy" [""; ""]}
| TFREAD LANGLE insidebounds RANGLE {  String.concat "fread" [""; ""]}
| TPTR LANGLE s = tstruct RANGLE {String.concat "" [s; "*"]}
| TPTR LANGLE s = insideptr RANGLE { String.concat "" [s; " *"]}
| TPTR LANGLE fp = fpointer RANGLE name = id_or_pid
  { let (ret,params) = fp in String.concat "" [ret; "(*"; name; ")"; params] }
(* "anonymous" function pointers *)
| TPTR LANGLE fp = fpointer RANGLE
  { let (ret,params) = fp in String.concat "" [ret; "(*"; ")"; params] }

fpointer:
| ret = param LPAREN lst = option(paramlist) RPAREN 
  { (ret, tostring lst) }
(* Can this be factored out? I don't know what I'm doing. ~ Matt *)
| ret = param LPAREN lst = option(paramlist) RPAREN annot
  { (ret, tostring lst ) }


paramlist:
| lst = separated_list(COMMA, param)
  { String.concat "" ["("; String.concat "," lst; ")"] }


param:
| p = checkedptr name = option(ID) b = option(bounds) { let _ = b in String.concat " " [p; tostring name] } 
| p = insideptr { p }

(* This could use the `list` production, but that causes a reduce reduce 
 * error. However, I beleive there are only 4 valid type qualifiers
 * (see https://en.wikipedia.org/wiki/C_data_types#Type_qualifiers) 
 * plus `unsigned`, so this should cover every case *)
(* TODO currently unused *)
qualed_type:
| c = TSTRUCT_str tstruct {c}
| c1 = ID c2 = ID? c3 = ID? c4 = ID? c5 = ID?
  { String.concat " " (List.fold_right (fun x y -> match (x,y) with 
    | (None, b) -> b
    | (Some a, b) -> a :: b)
  [Some c1; c2; c3; c4; c5] []) }

| c = checkedptr { c }

insideptr:
| TSTRUCT s = ID { String.concat "" ["struct " ; s]}
| c = ANY s = insideptr { String.concat "" [c; s]}
(* This is not properly capturing whitespace: it assumes there's a space between tokens, but that's not necessarily so. Need to fix lexer.  *)
| c = ID s = insideptr { let t = if is_tyvar c then "void" else c in String.concat " " [t; s] }
| c = ANY annot { c }
| c = ID annot { let t = if is_tyvar c then "void" else c in t }
| c = ANY { c }
| c = ID { let t = if is_tyvar c then "void" else c in t }

id_or_pid:
| c = ID { c }
| c = PID { c }

