" .gl_tags: generated from `ctags -f .tags --c++-kinds=+p /usr/include/GL/gl.h`
set tags+=.gl_tags

nnoremap <F4> :AsyncRun make clean && make<cr>
nnoremap <F5> :AsyncRun make clean && make && ./a0 < sphere.obj<CR>
