---------------------
Assertion Add(@1422, Level 0):
(let ((a!1 (>= (Length (Concat (AutomataDef abc) j)) 0))
      (a!2 (= (Length (Concat (AutomataDef abc) j))
              (+ (Length (AutomataDef abc)) (Length j)))))
  (and (= (Length (AutomataDef abc)) 3)
       (>= (Length (AutomataDef abc)) 0)
       (>= (Length j) 0)
       a!1
       a!2))
---------------------

---------------------
Assertion Add(@1422, Level 0):
(let ((a!1 (>= (Length (Concat (AutomataDef def) w)) 0))
      (a!2 (= (Length (Concat (AutomataDef def) w))
              (+ (Length (AutomataDef def)) (Length w)))))
  (and (= (Length (AutomataDef def)) 3)
       (>= (Length (AutomataDef def)) 0)
       (>= (Length w) 0)
       a!1
       a!2))
---------------------

---------------------
Assertion Add(@1422, Level 0):
(let ((a!1 (>= (Length (Concat (AutomataDef ghi123) w)) 0))
      (a!2 (= (Length (Concat (AutomataDef ghi123) w))
              (+ (Length (AutomataDef ghi123)) (Length w)))))
  (and (= (Length (AutomataDef ghi123)) 6)
       (>= (Length (AutomataDef ghi123)) 0)
       (>= (Length w) 0)
       a!1
       a!2))
---------------------





=================================================================================
** cb_new_eq(): @0
(NonDet_AutomataDef |(abcabc)*| 0)  = w




=================================================================================
** cb_new_eq(): @0
(NonDet_AutomataDef |(abcabc)*| 1)  = i




=================================================================================
** cb_new_eq(): @0
w  = (NonDet_AutomataDef |(abcabcabc)*| 2)




=================================================================================
** cb_new_eq(): @0
w  = (Concat (AutomataDef abc) j)




=================================================================================
** cb_new_eq(): @0
(Concat (AutomataDef def) w)  = g




=================================================================================
** cb_new_eq(): @0
(Concat (AutomataDef ghi123) w)  = t
