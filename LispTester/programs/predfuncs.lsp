(SET-MACRO-CHAR (QUOTE \') 
  (QUOTE (LAMBDA ()
    (LIST (QUOTE QUOTE) (READ)))
  )
)

; (SET-MACRO-CHAR '\"
;                    '(LAMBDA (CHAR LST)
;                        (LOOP
;                            (SETQ CHAR (READ-CHAR))
;                            ( (EQ CHAR '\")
;                                 (PACK (NREVERSE LST))
;                            )
;                            ( ( (EQ CHAR '\\) (PUSH (READ-CHAR) LST))
;                              (PUSH CHAR LST))
;                         )
;                     )
; )

(SET-MACRO-CHAR '\) '(LAMBDA () (BREAK '\) "Syntax Error") ))
(SET-MACRO-CHAR '\] '(LAMBDA () (BREAK '\] "Syntax Error") ))
(SET-MACRO-CHAR '\, '(LAMBDA () (BREAK '\, "Syntax Error") ))


(defun CAAR (lst) (car (car lst)))
(defun CDAR (lst) (cdr (car lst)))
(defun CADR (lst) (car (cdr lst)))
(defun CDDR (lst) (cdr (cdr lst)))


(defun CADDR (lst) (car (cddr lst)))
(defun CDDDR (lst) (cdr (cddr lst)))
(defun CAAAR (lst) (car (caar lst)))
(defun CDAAR (lst) (cdr (caar lst)))
(defun CADAR (lst) (car (cdar lst)))
(defun CDDAR (lst) (cdr (cdar lst)))
(defun CAADR (lst) (car (cadr lst)))
(defun CDADR (lst) (cdr (cadr lst)))


(defun CAADDR (lst) (car (car (cddr lst))))
(defun CADDDR (lst) (car (cdr (cddr lst))))
(defun CAAAAR (lst) (car (car (caar lst))))
(defun CADAAR (lst) (car (cdr (caar lst))))
(defun CAADAR (lst) (car (car (cdar lst))))
(defun CADDAR (lst) (car (cdr (cdar lst))))
(defun CAAADR (lst) (car (car (cadr lst))))
(defun CADADR (lst) (car (cdr (cadr lst))))

(defun CDADDR (lst) (cdr (car (cddr lst))))
(defun CDDDDR (lst) (cdr (cdr (cddr lst))))
(defun CDAAAR (lst) (cdr (car (caar lst))))
(defun CDDAAR (lst) (cdr (cdr (caar lst))))
(defun CDADAR (lst) (cdr (car (cdar lst))))
(defun CDDDAR (lst) (cdr (cdr (cdar lst))))
(defun CDAADR (lst) (cdr (car (cadr lst))))
(defun CDDADR (lst) (cdr (cdr (cadr lst))))

(DEFMACRO IF (C E1 E2)
    (LIST 'COND (LIST C E1) (LIST T E2))
)
; (DEFUN FUNCALL X
;     (APPLY (car x) (cdr x))
; )

; (DEFMACRO FUNCALL X
;     (list 'APPLY (list 'car (list 'quote x)) (list 'cdr (list 'quote x)))
; )

(DEFMACRO FUNCALL (fnc . args)
    (list 'APPLY fnc (list 'quote args))
)

(defun NOT (X)
     (if X nil T)
)

; (defun AND (nlambda lsta 
;    ((null (eval lsta)) NIL)
;    ((not (car (eval lsta))) (car (eval lsta)))
;    (apply 'AND (cdr (eval lsta)))
; )

; (defmacro AND 
;     (head . tail) (
;         (null head)
;     )
; )

;(defun AND (nlambda lsta  ((null (eval 'lsta)) NIL) (if (car lsta) (apply 'AND (cdr lsta)) nil)) )

(defun AND 
    (nlambda lsta 
        ((null lsta) NIL)
        ((null (cdr lsta)) (eval (car lsta)))
        ((eval (car lsta)) (apply 'AND (cdr lsta)))
        nil
    )
)

(defun OR 
    (nlambda lsta 
        ((null lsta) NIL)
        ((null (cdr lsta)) (eval (car lsta)))
        ((lambda (x tail) (if x x (apply 'OR tail)))
            (eval (car lsta))
            (cdr lsta)
        )
    )
)

; (defun OR (nlambda lsto
;    ((null (eval lsto)) NIL)
;    (if (car (eval lsto)) T (apply 'OR (cdr (eval lsto))))
; )
; )

(defun NTHCDR (N L)
    (if (minusp N)
        nil
        (loop
            ((= N 0) L)
            ((null L) nil)
            (setq N (- N 1))
            (setq L (cdr L))
        )
    )
)

(defun NTH (N L)
    (if (minusp N)
        nil
        (loop
            ((= N 0) (car L))
            ((null L) nil)
            (setq N (- N 1))
            (setq L (cdr L))
        )
    )
)

(defun SUBLIST (L N M)
    (if (and (listp L) (and (numberp N) (and (numberp M) (>= M N))))
        (progn
            (setq L (nthcdr N L))
            (setq M (- M N))
            (setq N 0)
            (setq C '())
            (if (>= M (length L)) (setq M (- (length L) 1)))
            (loop
                ((= (- N 1) M) C)
                (setq C (cons (nth M L) C))
                (setq M (- M 1))
            )
        )
        nil
    )
)

(defun ZEROP (X)
    (if (and (numberp X) (= X 0)) T nil)
)

(defun PLUSP (X)
    (if (and (numberp X) (> X 0)) T nil)
)

(defun MINUSP (X)
    (if (and (numberp X) (< X 0)) T nil)
)

;(defun ODDP (X)                          ;плохой алгоритм, возможно нужен integerp
;(if (= (mod X 2) 1) T nil)
;)
;(defun EVENP (X)
;(if (= (mod X 2) 0) T nil)
;)

(defun MOD (X L)                         ;плохая производительность
    (loop
        ;((not (and (numberp X) (numberp L))) nil)
        ((< (- X L) L) (- X L))
        (setq x (- x l))
    )
)

(DEFMACRO PUSH (OBJ SYM)
    (LIST 'SETQ SYM
        (LIST 'CONS OBJ SYM))
)

(DEFMACRO POP (SYM)
      (LIST 'PROG1
            (LIST 'CAR SYM)
            (LIST 'SETQ SYM (LIST 'CDR SYM)))
)

; (DEFUN MAPCAR (FUNC LST1 LST2)
;     ( (OR (NULL LST1) (NULL LST2)) NIL )
;     (CONS (FUNCALL FUNC (CAR LST1) (CAR LST2))
;     (MAPCAR FUNC (CDR LST1) (CDR LST2)))
; )

(DEFUN MAPLIST (FUNC LST1 LST2)
    ( (OR (NULL LST1) (NULL LST2)) NIL )
    (CONS (FUNCALL FUNC LST1 LST2)
        (MAPLIST FUNC (CDR LST1) (CDR LST2)))
)

(DEFUN MAPC (FUNC LST1 LST2)
    ( (OR (NULL LST1) (NULL LST2)) LST1 )
    ( FUNCALL FUNC (CAR LST1) (CAR LST2) )
    ( MAPC FUNC (CDR LST1) (CDR LST2) )
    LST1
)

(DEFUN MAPL (FUNC LST1 LST2)
    ( (OR (NULL LST1) (NULL LST2)) LST1 )
    ( FUNCALL FUNC LST1 LST2 )
    ( MAPL FUNC (CDR LST1) (CDR LST2) )
    LST1
)

(DEFUN NREVERSE (LAMBDA (LST OBJ)
      (COND ( (ATOM LST) OBJ )
            (  T  (NREVERSE (CDR LST) (RPLACD LST OBJ)) )
      )
   ))



; реализация с сайта (работает только на 2 аргумента)
; (DEFUN NCONC (LST1 LST2)
;     ( (ATOM LST1) LST2 )
;     ( RPLACD (LAST LST1) LST2 )
;     LST1
; )

; своя реализация: нужно подоптимизировать (+на дефмакро написать) (сделано просто чтобы заработало, влияет на `)
(DEFUN NCONC X
    ( (null x) nil)
    ( (null (cdr x)) (car x))
    ( (ATOM (car x)) (apply 'NCONC (cdr x)))
    ( RPLACD (LAST (car x)) (cadr x))
    (setq x (append (list (car x)) (cddr x)))
    (apply 'NCONC x)
)



(DEFUN ADD1 (N)
      ( (NUMBERP N) (+ N 1))
      ( BREAK (LIST 'ADD1 N) '"Nonnumeric Argument")
)

(DEFUN LIST* LST
      ( (NULL LST) NIL )
      ( (NULL (CDR LST)) (CAR LST) )
      ( CONS (CAR LST) (APPLY 'LIST*  (CDR LST)) )
   )

; не нашел описания, сделал чисто по паре тестов
(DEFMACRO NEQ (A B)
     (LIST 'NOT (list 'EQ A B))
)

(DEFUN COPY-LIST (LST)
      ( (ATOM LST) LST )
      ( CONS (CAR LST) (COPY-LIST (CDR LST)) )
)


(SETQ *BACKQUOTE-CHAR* '\`)	;Determines the backquote macro character

(SETQ *LPAR* '\()
(SETQ *RPAR* '\))

(DEFUN READ-BACKQUOTE (PAREN-DEPTH
; This backquote facility conforms to the Common LISP standard as
; described in Section 22.1.3 of Common LISP by Steele [1984].
; It was contributed by Fujimasa Kouno of Tokushima, Japan.
    OBJ CHAR FORM-LIST)
  (SETQ CHAR (PEEK-CHAR T))

  ((EQ CHAR *BACKQUOTE-CHAR*)
    ((ZEROP PAREN-DEPTH)
      (READ) )
    (LIST 'LIST (READ)) )

  ((EQ CHAR '\')
    (READ-CHAR)
    ((ZEROP PAREN-DEPTH)
      (LIST 'QUOTE (LIST 'QUOTE (EVAL (READ-BACKQUOTE PAREN-DEPTH)))) )
    (LIST 'LIST (LIST* 'LIST ''QUOTE (CDR (READ-BACKQUOTE PAREN-DEPTH)))) )

  ((EQ CHAR '\.)
    ((ZEROP PAREN-DEPTH)
      (BREAK CHAR "Syntax Error") )
    (READ-CHAR)
    (SETQ CHAR (PEEK-CHAR T))
    ((EQ CHAR '\,)
      (READ-CHAR)
      (SETQ CHAR (PEEK-CHAR))
      ((OR (EQ CHAR '@) (EQ CHAR '\.))
	(BREAK CHAR "Syntax Error") )
      (SETQ OBJ (READ)
	    CHAR (PEEK-CHAR T) )
      ((EQ CHAR *RPAR*) OBJ)
      (BREAK CHAR "Syntax Error") )
    (SETQ OBJ (LIST 'QUOTE (READ))
	  CHAR (PEEK-CHAR T) )
    ((EQ CHAR *RPAR*) OBJ)
    (BREAK CHAR "Syntax Error") )

  ((EQ CHAR '\,)
    (READ-CHAR)
    (SETQ CHAR (PEEK-CHAR))
    ((AND (EQ CHAR '\@) (NEQ PAREN-DEPTH 0))
      (READ-CHAR)
      (SETQ OBJ (READ))
      ((EQ (PEEK-CHAR T) *RPAR*) OBJ)
      (LIST 'COPY-LIST OBJ) )
    ((AND (EQ CHAR '\.) (NEQ PAREN-DEPTH 0))
      (READ-CHAR)
      (READ) )
    ((OR (EQ CHAR '\@) (EQ CHAR '\.))
      (BREAK CHAR "Syntax Error") )
    ((ZEROP PAREN-DEPTH)
      (READ) )
    (LIST 'LIST (READ)) )

  ((EQ CHAR *RPAR*)
    (READ-CHAR)
    NIL)

  ((EQ CHAR *LPAR*)
    (READ-CHAR)
    (LOOP
      ((NULL (SETQ OBJ (READ-BACKQUOTE (ADD1 PAREN-DEPTH))))
	((ZEROP PAREN-DEPTH)
	  (CONS 'NCONC (NREVERSE FORM-LIST)) )
	(LIST 'LIST (CONS 'NCONC (NREVERSE FORM-LIST))) )
      (PUSH OBJ FORM-LIST) ) )

  ((ZEROP PAREN-DEPTH)
    (LIST 'QUOTE (READ)) )
  (LIST 'LIST (LIST 'QUOTE (READ))) )

(SET-MACRO-CHAR *BACKQUOTE-CHAR* '(LAMBDA ()
  (READ-BACKQUOTE 0) ))

(DEFMACRO IF (C E1 E2) 
  `(COND (,C ,E1) (T ,E2))
)