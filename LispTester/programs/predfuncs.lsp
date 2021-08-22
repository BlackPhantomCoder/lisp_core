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

(DEFUN FUNCALL X
    (APPLY (car x) (cdr x))
)

(defun NOT (X)
     (if X nil T)
)

(defun AND (nlambda lsta 
   ((null (eval lsta)) NIL)
   (if (car (eval lsta)) (apply 'AND (cdr (eval lsta))) nil))
)
(defun OR (nlambda lsto
   ((null (eval lsto)) NIL)
   (if (car (eval lsto)) T (apply 'OR (cdr (eval lsto))))
)
)

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