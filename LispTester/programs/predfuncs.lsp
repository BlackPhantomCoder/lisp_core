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

(defun APPEND (lst tail)
    ((null lst) tail)
    (cons (car lst) (append (cdr lst) tail))
)

(defun IF 
    (nlambda (predicate then else)
        ((eval predicate) (eval then))
        (eval else)
    )
)

; (DEFUN FUNCALL (FUNC ARG1 ARG2...ARGN)
;     (APPLY FUNC ARG1 ARG2...ARGN NIL)
; )