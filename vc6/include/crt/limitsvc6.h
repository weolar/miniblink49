
#ifndef __numeric_limits_vc6__
#define __numeric_limits_vc6__

#if USING_VC6RT == 1

#define CHAR_BIT      8
#define _LLONG_MAX 0x7fffffffffffffff

namespace std {
    struct _CRTIMP _Num_int64_base : public _Num_base {
        _STCONS(bool, is_bounded, true);
        _STCONS(bool, is_exact, true);
        _STCONS(bool, is_integer, true);
        _STCONS(bool, is_modulo, true);
        _STCONS(bool, is_specialized, true);
        _STCONS(int, radix, 2);
    };

    template <>
    class _CRTIMP numeric_limits<__int64> : public _Num_int64_base {
    public:
        typedef __int64 _Ty;

        static _Ty(min)()
        {	// return minimum value
            return (-_LLONG_MAX - 1);
        }

        static _Ty(max)()
        {	// return maximum value
            return (_LLONG_MAX);
        }

        static _Ty lowest()
        {	// return most negative value
            return ((min)());
        }

        static _Ty epsilon()
        {	// return smallest effective increment from 1.0
            return (0);
        }

        static _Ty round_error()
        {	// return largest rounding error
            return (0);
        }

        static _Ty denorm_min()
        {	// return minimum denormalized value
            return (0);
        }

        static _Ty infinity()
        {	// return positive infinity
            return (0);
        }

        static _Ty quiet_NaN()
        {	// return non-signaling NaN
            return (0);
        }

        static _Ty signaling_NaN()
        {	// return signaling NaN
            return (0);
        }

        _STCONS(bool, is_signed, true);
        _STCONS(int, digits, CHAR_BIT * sizeof(__int64) - 1);
        _STCONS(int, digits10, (CHAR_BIT * sizeof(__int64) - 1)
            * 301L / 1000);
    };
}

#endif // #if USING_VC6RT == 1

#endif // __numeric_limits_vc6__