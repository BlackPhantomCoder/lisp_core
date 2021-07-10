;     1. �������� �㬬� �����⮢ 楫�� �ᥫ � �������� ���ࢠ��
;           ���祭�� �� M �� N �����⥫쭮.
(defun sum_sqrs (a b)
    (if (> a b) 
        0
        (+ 
            (* a a) 
            (sum_sqrs (+ a 1) b)
        )
    )
)


;     3. ��।����� �������訩 ��騩 ����⥫� ���� �������� �ᥫ.
(defun nod (a b)
    (cond
        ((> a b) (nod (- a b) b))
        ((< a b) (nod a (- b a)))
        (T a)
    )
)


;     2. ��।����� �������襥 ��饥 ��⭮� ���� �������� �ᥫ.
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
;     4. ���᫨�� ������� ��७� �� ��������� �᫠.

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




;     5. ���� ��� 童�� �鸞 �������� ࠢ�� 1 � 2. ����� ᫥���騩
;           童� �鸞 ࠢ�� �㬬� ���� �।����.  ��।����� �㭪��,
;           �� ������� N-� 童� �鸞 ��������.
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