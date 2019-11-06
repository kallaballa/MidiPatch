section .rodata

global __binary_index_min_pack_start;
global __binary_index_min_pack_end;

__binary_index_min_pack_start:     incbin "index.min.pack"
__binary_index_min_pack_end:    db 0x88
