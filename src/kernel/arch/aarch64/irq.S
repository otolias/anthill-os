.global irq_disable
irq_disable:
    msr daifset, #2
    ret

.global irq_enable
irq_enable:
    msr daifclr, #2
    ret
