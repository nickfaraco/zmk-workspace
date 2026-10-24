/*
    Bindings of the keys that exist on some boards only, selected by
        #define ALPHA_23332 1    top outer corners (LT4, RT4) active
    or
        #define ALPHA_13332 1    (default) none of them active
    The numbers count the keys per column, from the outer to the inner one.
    Everything bound here must also be reachable through a combo, so that the
    28-key layout stays complete.
*/

#if ALPHA_23332
#define LTL &kp V
#define RTR &single_to_doublequote
#else
#define LTL &none
#define RTR &none
#endif

// bottom outer and bottom inner keys: unused on every board
#define LBL &none
#define LBR &none
#define RBL &none
#define RBR &none
