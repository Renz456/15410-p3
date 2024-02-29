/**
 * @file fault_asm_wrappers.S
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief 
 * 
 * @bug 
 * 
 */

#include <handler.h>

.global de_wrapper
.global db_wrapper
.global nmi_wrapper
.global bp_wrapper
.global of_wrapper
.global br_wrapper
.global ud_wrapper
.global nm_wrapper
.global df_wrapper
.global cop_wrapper
.global ts_wrapper
.global np_wrapper
.global ss_wrapper
.global gp_wrapper
.global pf_wrapper  
.global mf_wrapper
.global ac_wrapper
.global mc_wrapper

/* Not making a SIMD wrapper */

de_wrapper:
    pusha
    push $0
    call fault_handler
    popa
    iret
    
db_wrapper:
    pusha
    push $1
    call fault_handler
    popa
    iret

nmi_wrapper:
    pusha
    push $2
    call fault_handler
    popa
    iret

bp_wrapper:
    pusha
    push $3
    call fault_handler
    popa
    iret

of_wrapper:
    pusha
    push $4
    call fault_handler
    popa
    iret

br_wrapper:
    pusha
    push $5
    call fault_handler
    popa
    iret

ud_wrapper:
    pusha
    push $6
    call fault_handler
    popa
    iret

nm_wrapper:
    pusha
    push $7 
    call fault_handler
    popa
    iret

df_wrapper:
    pusha
    push $8
    call fault_handler
    popa
    iret

cop_wrapper:
    pusha
    push $9
    call fault_handler
    popa
    iret

ts_wrapper:
    pusha
    push $10 
    call fault_handler
    popa
    iret

np_wrapper:
    pusha
    push $11 
    call fault_handler
    popa
    iret

ss_wrapper:
    pusha
    push $12 
    call fault_handler
    popa
    iret

gp_wrapper:
    pusha
    push $13 
    call fault_handler
    popa
    iret

pf_wrapper:
    pusha
    push $14
    call fault_handler
    popa
    iret

mf_wrapper:
    pusha
    push $16     
    call fault_handler
    popa
    iret

ac_wrapper:
    pusha
    push $17 
    call fault_handler
    popa
    iret

mc_wrapper:
    pusha
    push $18 
    call fault_handler
    popa
    iret
