(defun sum_sqrs (a b)
    (if (> a b) 
        0
        (+ 
            (* a a) 
            (sum_sqrs (+ a 1) b)
        )
    )
)

(defun nod (a b)
    (cond
        ((> a b) (nod (- a b) b))
        ((< a b) (nod a (- b a)))
        (T a)
    )
)

(defun nok (a b)
    (/
        (* a b)
        (nod a b)
    )
)

(defun abs (a)
    (if (> a 0)
        a
        (- a)
    )
)

(defun sqrt_r (a xn e)
    (
        (lambda (xn xn1 a e) 
            (if 
                (<= 
                    (abs (- xn xn1))
                    e
                )
                xn1
                (sqrt_r a xn1 e)
            )
        )
        xn
        (/
            (+
                xn
                (/ a xn)
            )
            2
        )
        a
        e
    )
)

(defun sqrt (x e)
    (sqrt_r x x e)
)

(defun fib (n)
    (cond
        ((= n 1) 1)
        ((= n 2) 2)
        (T 
            (+
                (fib (- n 1))
                (fib (- n 2))
            )
        )
    )
)

(defun fib2_rec (n)
    (cond 
        ((<= n 0) nil)
        ((= n 1) '(1 0))
        ((= n 2) '(1 1))
        (T
            (
                (lambda (lst) 
                    (list (+ (car lst) (cadr lst)) (car lst))
                )
                (fib2_rec (- n 1))
            )
        )
    )
)

(defun fib2 (n)
   (
       (lambda (lst)
            (if (NULL lst)
                nil
                (
                    (lambda (lst) 
                        (+ (car lst) (cadr lst))
                    )
                    lst
                )
            )
        )
        (fib2_rec n)
    )
)




(defun fib3_rec (one two n)
    (if (= n 0)
        two
        (fib3_rec two (+ one two) (- n 1))
    )
)

(defun fib3 (n)
    (cond 
        ((<= n 0) nil)
        ((= n 1) 1)
        ((= n 2) 2)
        (T (fib3_rec 1 2 (- n 2)))
    )
)

(defun mytest_print_y ()  (print a) (print b) (print c) nil)
(defun mytest_a (a) (mytest_b (+ a 1)))
(defun mytest_b (b) (mytest_c (+ a 1)))
(defun mytest_c (c) (mytest_print_y))



(defun gen_next (name_val) 
    (list  lambda (list 'name_val name_val)
        (list 'if '(equal name_val 'NIL11111)
            'T
            (list 'progn
                (list 'if '(equal name_val 'NIL11)
                    (list 'progn
                        (list 'setq name_val '5)
                        (list 'print 'setq)
                    )
                )
                '(print NIL11)
                '(setq name_val (pack (append (unpack name_val) (list 1))))
                '(print name_val)
                '(apply (gen_next name_val) (list name_val))
            )
        )
    )
)



; (lambda (name_val NIL11)
;     (if (equal name_val 'NIL11111)
;         T
;         (progn
;             (if (equal name_val 'NIL11)
;                 (progn
;                     (setq NIL11 5)
;                     (print 'setq)
;                 )
;             )
;             (print NIL11)
;             (setq name_val (pack (append (unpack name_val) (list 1))))
;             (print name_val)
;             (apply (gen_next name_val) (list name_val))
;         )
;     )
; )

;(apply '(lambda (X L) (* X (car L))) '(3 (4 5 6)))


(DEFUN OVER (LAMBDA (N K C) (IF (= N C) (PROGN (PRINT (+ K 1)) (OVER 0 (+ K 1) C)) (OVER (+ 1 N) K C))))
(DEFUN TOVER (C) (OVER 0 0 C))
(DEFUN STOVER (NLAMBDA (N0 N1 N2 N3 N4 N5 N6 N7 N8 N9 N10 N11 N12 N13 N14 N15) (STOVER N0 N1 N2 N3 N4 N5 N6 N7 N8 N9 N10 N11 N12 N13 N14 N15)))