type alias A = C
type alias B = A
type alias C = B # compile error, Cyclic type alias detected for C and B
