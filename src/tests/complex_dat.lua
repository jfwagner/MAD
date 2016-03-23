local M = {}

local inf = 1.0/0.0

M.z = {
 1                      +  0i                     ,
 9.2387953251128676e-1  +  3.8268343236508977e-1i ,
 7.0710678118654752e-1  +  7.0710678118654752e-1i ,
 3.8268343236508977e-1  +  9.2387953251128676e-1i ,
 0                      +  1i                     ,
-3.8268343236508977e-1  +  9.2387953251128676e-1i ,
-7.0710678118654752e-1  +  7.0710678118654752e-1i ,
-9.2387953251128676e-1  +  3.8268343236508977e-1i ,
-1                      +  0i                     ,
-9.2387953251128676e-1  + -3.8268343236508977e-1i ,
-7.0710678118654752e-1  + -7.0710678118654752e-1i ,
-3.8268343236508977e-1  + -9.2387953251128676e-1i ,
 0                      + -1i                     ,
 3.8268343236508977e-1  + -9.2387953251128676e-1i ,
 7.0710678118654752e-1  + -7.0710678118654752e-1i ,
 9.2387953251128676e-1  + -3.8268343236508977e-1i ,
} 

M.exp = {
 2.7182818284590452     +  0i                     ,
 2.3368315124516664     +  9.4063915499640848e-1i ,
 1.5418634570456317     +  1.3175384087798810i    ,
 8.8372964097847180e-1  +  1.1699593288600548i    ,
 5.4030230586813972e-1  +  8.4147098480789651e-1i ,
 4.1107854986689264e-1  +  5.4422208106376100e-1i ,
 3.7485280862038230e-1  +  3.2031563543421550e-1i ,
 3.6826109035395399e-1  +  1.4823524888415080e-1i ,
 3.6787944117144232e-1  +  0i                     ,
 3.6826109035395399e-1  + -1.4823524888415080e-1i ,
 3.7485280862038230e-1  + -3.2031563543421550e-1i ,
 4.1107854986689264e-1  + -5.4422208106376100e-1i ,
 5.4030230586813972e-1  + -8.4147098480789651e-1i ,
 8.8372964097847180e-1  + -1.1699593288600548i    ,  
 1.5418634570456317     + -1.3175384087798810i    ,
 2.3368315124516664     + -9.4063915499640848e-1i ,
}  

M.log = {
  0i                     ,
  3.9269908169872415e-1i ,
  7.8539816339744831e-1i ,
  1.1780972450961725i    ,
  1.5707963267948966i    ,
  1.9634954084936208i    ,
  2.3561944901923449i    ,
  2.7488935718910691i    ,
  3.1415926535897932i    ,
 -2.7488935718910691i    ,
 -2.3561944901923449i    ,
 -1.9634954084936208i    ,
 -1.5707963267948966i    ,
 -1.1780972450961725i    ,
 -7.8539816339744831e-1i ,
 -3.9269908169872415e-1i ,
}

M.sqrt = {
 1                      +  0i                     ,
 9.8078528040323045e-1  +  1.9509032201612827e-1i ,
 9.2387953251128676e-1  +  3.8268343236508977e-1i ,
 8.3146961230254524e-1  +  5.5557023301960222e-1i ,
 7.0710678118654752e-1  +  7.0710678118654752e-1i ,
 5.5557023301960222e-1  +  8.3146961230254524e-1i ,
 3.8268343236508977e-1  +  9.2387953251128676e-1i ,
 1.9509032201612827e-1  +  9.8078528040323045e-1i ,
 0                      +  1i                     ,
 1.9509032201612827e-1  + -9.8078528040323045e-1i ,
 3.8268343236508977e-1  + -9.2387953251128676e-1i ,
 5.5557023301960222e-1  + -8.3146961230254524e-1i ,
 7.0710678118654752e-1  + -7.0710678118654752e-1i ,
 8.3146961230254524e-1  + -5.5557023301960222e-1i ,
 9.2387953251128676e-1  + -3.8268343236508977e-1i ,
 9.8078528040323045e-1  + -1.9509032201612827e-1i ,
}

M.sin = {
 8.4147098480789651e-1  +  0i                     ,
 8.5709070496190789e-1  +  2.3632554555578958e-1i ,
 8.1892702210704826e-1  +  5.8350532421262472e-1i ,
 5.4443720194027964e-1  +  9.8428521104885620e-1i ,
 0                      +  1.1752011936438015i    ,
-5.4443720194027964e-1  +  9.8428521104885620e-1i ,
-8.1892702210704826e-1  +  5.8350532421262472e-1i ,
-8.5709070496190789e-1  +  2.3632554555578958e-1i ,
-8.4147098480789651e-1  +  0i                     ,
-8.5709070496190789e-1  + -2.3632554555578958e-1i ,
-8.1892702210704826e-1  + -5.8350532421262472e-1i ,
-5.4443720194027964e-1  + -9.8428521104885620e-1i ,
 0                      + -1.1752011936438015i    ,
 5.4443720194027964e-1  + -9.8428521104885620e-1i ,
 8.1892702210704826e-1  + -5.8350532421262472e-1i ,
 8.5709070496190789e-1  + -2.3632554555578958e-1i ,
} 

M.cos = {
 5.4030230586813972e-1  +  0i                     ,
 6.4740409542268222e-1  + -3.1286862389814690e-1i ,
 9.5835813283300702e-1  + -4.9861138667283276e-1i ,
 1.3525463014028102     + -3.9620195305612884e-1i ,
 1.5430806348152438     +  0i                     ,
 1.3525463014028102     +  3.9620195305612884e-1i ,
 9.5835813283300702e-1  +  4.9861138667283276e-1i ,
 6.4740409542268222e-1  +  3.1286862389814690e-1i ,
 5.4030230586813972e-1  +  0i                     ,
 6.4740409542268222e-1  + -3.1286862389814690e-1i ,
 9.5835813283300702e-1  + -4.9861138667283276e-1i ,
 1.3525463014028102     + -3.9620195305612884e-1i ,
 1.5430806348152438     +  0i                     ,
 1.3525463014028102     +  3.9620195305612884e-1i ,
 9.5835813283300702e-1  +  4.9861138667283276e-1i ,
 6.4740409542268222e-1  +  3.1286862389814690e-1i ,
} 

M.tan = {
 1.5574077246549022     +  0i                     ,
 9.3022758246618703e-1  +  8.1458330727700386e-1i ,
 4.2318427386994581e-1  +  8.2903227364117000e-1i ,
 1.7438995923784657e-1  +  7.7881167720450291e-1i ,
 0                      +  7.6159415595576489e-1i ,
-1.7438995923784657e-1  +  7.7881167720450291e-1i ,
-4.2318427386994581e-1  +  8.2903227364117000e-1i ,
-9.3022758246618703e-1  +  8.1458330727700386e-1i ,
-1.5574077246549022     +  0i                     ,
-9.3022758246618703e-1  + -8.1458330727700386e-1i ,
-4.2318427386994581e-1  + -8.2903227364117000e-1i ,
-1.7438995923784657e-1  + -7.7881167720450291e-1i ,
 0                      + -7.6159415595576489e-1i ,
 1.7438995923784657e-1  + -7.7881167720450291e-1i ,
 4.2318427386994581e-1  + -8.2903227364117000e-1i ,
 9.3022758246618703e-1  + -8.1458330727700386e-1i ,
} 

M.sinh = {
 1.1752011936438015     +  0i                     ,
 9.8428521104885620e-1  +  5.4443720194027964e-1i ,
 5.8350532421262472e-1  +  8.1892702210704826e-1i ,
 2.3632554555578958e-1  +  8.5709070496190789e-1i ,
 0                      +  8.4147098480789651e-1i ,
-2.3632554555578958e-1  +  8.5709070496190789e-1i ,
-5.8350532421262472e-1  +  8.1892702210704826e-1i ,
-9.8428521104885620e-1  +  5.4443720194027964e-1i ,
-1.1752011936438015     +  0i                     ,
-9.8428521104885620e-1  + -5.4443720194027964e-1i ,
-5.8350532421262472e-1  + -8.1892702210704826e-1i ,
-2.3632554555578958e-1  + -8.5709070496190789e-1i ,
 0                      + -8.4147098480789651e-1i ,
 2.3632554555578958e-1  + -8.5709070496190789e-1i ,
 5.8350532421262472e-1  + -8.1892702210704826e-1i ,
 9.8428521104885620e-1  + -5.4443720194027964e-1i ,
} 

M.cosh = {
 1.5430806348152438     +  0i                     , 
 1.3525463014028102     +  3.9620195305612884e-1i ,
 9.5835813283300702e-1  +  4.9861138667283276e-1i ,
 6.4740409542268222e-1  +  3.1286862389814690e-1i ,
 5.4030230586813972e-1  +  0i                     ,
 6.4740409542268222e-1  + -3.1286862389814690e-1i ,
 9.5835813283300702e-1  + -4.9861138667283276e-1i ,
 1.3525463014028102     + -3.9620195305612884e-1i ,
 1.5430806348152438     +  0i                     ,
 1.3525463014028102     +  3.9620195305612884e-1i ,
 9.5835813283300702e-1  +  4.9861138667283276e-1i ,
 6.4740409542268222e-1  +  3.1286862389814690e-1i ,
 5.4030230586813972e-1  +  0i                     ,
 6.4740409542268222e-1  + -3.1286862389814690e-1i ,
 9.5835813283300702e-1  + -4.9861138667283276e-1i ,
 1.3525463014028102     + -3.9620195305612884e-1i ,
} 

M.tanh = {
 7.6159415595576489e-1  +  0i                     ,
 7.7881167720450291e-1  +  1.7438995923784657e-1i ,
 8.2903227364117000e-1  +  4.2318427386994581e-1i ,
 8.1458330727700386e-1  +  9.3022758246618703e-1i ,
 0                      +  1.5574077246549022i    ,
-8.1458330727700386e-1  +  9.3022758246618703e-1i ,
-8.2903227364117000e-1  +  4.2318427386994581e-1i ,
-7.7881167720450291e-1  +  1.7438995923784657e-1i ,
-7.6159415595576489e-1  +  0i                     ,
-7.7881167720450291e-1  + -1.7438995923784657e-1i ,
-8.2903227364117000e-1  + -4.2318427386994581e-1i ,
-8.1458330727700386e-1  + -9.3022758246618703e-1i ,
 0                      + -1.5574077246549022i    ,
 8.1458330727700386e-1  + -9.3022758246618703e-1i ,
 8.2903227364117000e-1  + -4.2318427386994581e-1i ,
 7.7881167720450291e-1  + -1.7438995923784657e-1i ,
} 

M.asin = {
 1.5707963267948966     +  0i                     ,
 9.0381873872602644e-1  +  5.8472070562908375e-1i ,
 5.7185887020121025e-1  +  7.6428545974049908e-1i ,
 2.7952527383989980e-1  +  8.5366030642857035e-1i ,
 0                      +  8.8137358701954303e-1i ,
-2.7952527383989980e-1  +  8.5366030642857035e-1i ,
-5.7185887020121025e-1  +  7.6428545974049908e-1i ,
-9.0381873872602644e-1  +  5.8472070562908375e-1i ,
-1.5707963267948966     +  0i                     ,
-9.0381873872602644e-1  + -5.8472070562908375e-1i ,
-5.7185887020121025e-1  + -7.6428545974049908e-1i ,
-2.7952527383989980e-1  + -8.5366030642857035e-1i ,
 0                      + -8.8137358701954303e-1i ,
 2.7952527383989980e-1  + -8.5366030642857035e-1i ,
 5.7185887020121025e-1  + -7.6428545974049908e-1i ,
 9.0381873872602644e-1  + -5.8472070562908375e-1i ,
} 

M.acos = {
 0                      +  0i                     ,
 6.6697758806887018e-1  + -5.8472070562908375e-1i ,
 9.9893745659368637e-1  + -7.6428545974049908e-1i ,
 1.2912710529549968     + -8.5366030642857035e-1i ,
 1.5707963267948966     + -8.8137358701954303e-1i ,
 1.8503216006347964     + -8.5366030642857035e-1i ,
 2.1426551969961069     + -7.6428545974049908e-1i ,
 2.4746150655209231     + -5.8472070562908375e-1i ,
 3.1415926535897932     +  0i                     ,
 2.4746150655209231     +  5.8472070562908375e-1i ,
 2.1426551969961069     +  7.6428545974049908e-1i ,
 1.8503216006347964     +  8.5366030642857035e-1i ,
 1.5707963267948966     +  8.8137358701954303e-1i ,
 1.2912710529549968     +  8.5366030642857035e-1i ,
 9.9893745659368637e-1  +  7.6428545974049908e-1i ,
 6.6697758806887018e-1  +  5.8472070562908375e-1i ,
} 

M.atan = {
 7.8539816339744831e-1  +  0i                     ,
 7.8539816339744831e-1  +  2.0159985958075575e-1i ,
 7.8539816339744831e-1  +  4.4068679350977151e-1i ,
 7.8539816339744831e-1  +  8.0744545808654764e-1i ,
 0                      +  inf*1i                 ,
-7.8539816339744831e-1  +  8.0744545808654764e-1i ,
-7.8539816339744831e-1  +  4.4068679350977151e-1i ,
-7.8539816339744831e-1  +  2.0159985958075575e-1i ,
-7.8539816339744831e-1  +  0i                     ,
-7.8539816339744831e-1  + -2.0159985958075575e-1i ,
-7.8539816339744831e-1  + -4.4068679350977151e-1i ,
-7.8539816339744831e-1  + -8.0744545808654764e-1i ,
 0                      + -inf*1i                 ,
 7.8539816339744831e-1  + -8.0744545808654764e-1i ,
 7.8539816339744831e-1  + -4.4068679350977151e-1i ,
 7.8539816339744831e-1  + -2.0159985958075575e-1i ,
} 

M.asinh = {
 8.8137358701954303e-1  +  0i                     ,
 8.5366030642857035e-1  +  2.7952527383989980e-1i ,
 7.6428545974049908e-1  +  5.7185887020121025e-1i ,
 5.8472070562908375e-1  +  9.0381873872602644e-1i ,
 0                      +  1.5707963267948966i    ,
-5.8472070562908375e-1  +  9.0381873872602644e-1i ,
-7.6428545974049908e-1  +  5.7185887020121025e-1i ,
-8.5366030642857035e-1  +  2.7952527383989980e-1i ,
-8.8137358701954303e-1  +  0i                     ,
-8.5366030642857035e-1  + -2.7952527383989980e-1i ,
-7.6428545974049908e-1  + -5.7185887020121025e-1i ,
-5.8472070562908375e-1  + -9.0381873872602644e-1i ,
 0                      + -1.5707963267948966i    ,
 5.8472070562908375e-1  + -9.0381873872602644e-1i ,
 7.6428545974049908e-1  + -5.7185887020121025e-1i , 
 8.5366030642857035e-1  + -2.7952527383989980e-1i ,
}  

M.acosh = {
 0                      +  0i                     ,
 5.8472070562908375e-1  +  6.6697758806887018e-1i ,
 7.6428545974049908e-1  +  9.9893745659368637e-1i ,
 8.5366030642857035e-1  +  1.2912710529549968i    ,
 8.8137358701954303e-1  +  1.5707963267948966i    ,
 8.5366030642857035e-1  +  1.8503216006347964i    ,
 7.6428545974049908e-1  +  2.1426551969961069i    ,
 5.8472070562908375e-1  +  2.4746150655209231i    ,
 0                      +  3.1415926535897932i    ,
 5.8472070562908375e-1  + -2.4746150655209231i    ,
 7.6428545974049908e-1  + -2.1426551969961069i    ,
 8.5366030642857035e-1  + -1.8503216006347964i    ,
 8.8137358701954303e-1  + -1.5707963267948966i    ,
 8.5366030642857035e-1  + -1.2912710529549968i    ,
 7.6428545974049908e-1  + -9.9893745659368637e-1i ,
 5.8472070562908375e-1  + -6.6697758806887018e-1i ,
} 

M.atanh = {
 inf                    +  0i                     ,
 8.0744545808654764e-1  +  7.8539816339744831e-1i ,
 4.4068679350977151e-1  +  7.8539816339744831e-1i ,
 2.0159985958075575e-1  +  7.8539816339744831e-1i ,
 0                      +  7.8539816339744831e-1i ,
-2.0159985958075575e-1  +  7.8539816339744831e-1i ,
-4.4068679350977151e-1  +  7.8539816339744831e-1i ,
-8.0744545808654764e-1  +  7.8539816339744831e-1i ,
-inf                    +  0i                     ,
-8.0744545808654764e-1  + -7.8539816339744831e-1i ,
-4.4068679350977151e-1  + -7.8539816339744831e-1i ,
-2.0159985958075575e-1  + -7.8539816339744831e-1i ,
 0                      + -7.8539816339744831e-1i ,
 2.0159985958075575e-1  + -7.8539816339744831e-1i ,
 4.4068679350977151e-1  + -7.8539816339744831e-1i ,
 8.0744545808654764e-1  + -7.8539816339744831e-1i ,
}

return M
