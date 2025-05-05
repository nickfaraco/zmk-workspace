/*
    to select which keys configuration the keyboard uses
        #define ALPHA_13332 1
            or
        #define ALPHA_23332 1
    If none of the above is added, a full 3x5 matrix is defined for each half
    of the split keyboard
*/

#if ALPHA_13332
#define LTL &none
#define LBL &none
#define LBR &none
#define RTR &none
#define RBL &none
#define RBR &none
#elif ALPHA_23332
#define LTL &kp ESC
#define LBL &kp V
#define LBR &none
#define RTR &kp RET
#define RBL &none
#define RBR &mm_quote
#else // full 3x5 matrix
#define LTL &kp B
#define LBL &kp Q
#define LBR &kp V
#define RTR &kp J
#define RBL &kp K
#define RBR &kp SLASH
#endif