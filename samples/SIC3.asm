test     start   1000
first    lda     zero
         sta     len
         lda     zero
         sta     result
         jsub    strlen
         jsub    strcpy
         jsub    strcmp
         lda     result
         j       end
strlen   ldx     len
         ldch    str2
         comp    null
         jeq     back
         lda     len
         add     one
         sta     len
         j       strlen
back     rsub
strcpy   ldx     zero
cloop    ldch    str2
         stch    str1
         tix     len
         jlt     cloop
         rsub
strcmp   ldx     zero
cploop   ldch    str2
         comp    str3
         jlt     small
         jgt     big
         jeq     equal
small    lda     minus
         sta     result
         j       cend
big      lda     plus
         sta     result
         j       cend
equal    tix     len
         jlt     cploop
cend     rsub
zero     word    0
one      word    1
minus    word    -1
plus     word    1
len      resw    1
result   resw    1
str1     resb    10
str2     byte    c'test string'
null     word    0
str3     byte    c'test string'
end      end     first