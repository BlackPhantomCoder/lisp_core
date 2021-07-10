;     1. Подсчитать сумму квадратов целых чисел в заданном интервале
;           значений от M до N включительно.
(defun sum_sqrs (a b)
    (if (> a b) 
        0
        (+ 
            (* a a) 
            (sum_sqrs (+ a 1) b)
        )
    )
)


;     3. Определить наибольший общий делитель двух заданных чисел.
(defun nod (a b)
    (cond
        ((> a b) (nod (- a b) b))
        ((< a b) (nod a (- b a)))
        (T a)
    )
)


;     2. Определить наименьшее общее кратное двух заданных чисел.
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
;     4. Вычислить квадратный корень из заданного числа.

; xn1 = (xn + a/xn)/2
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




;     5. Первые два члена ряда Фибоначчи равны 1 и 2. Каждый следующий
;           член ряда равен сумме двух предыдущих.  Определить функцию,
;           вы числяющую N-ый член ряда Фибоначчи.
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

(fib 5)