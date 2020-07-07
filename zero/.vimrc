command! MakeTags :AsyncRun ctags -f .tags -R . --c++-kinds=+p

" .gl_tags: generated from `ctags -f .tags --c++-kinds=+p /usr/include/GL/gl.h`
set tags+=.gl_tags

nnoremap <F4> :w<cr>:AsyncRun make<cr>
nnoremap <F5> :w<cr>:AsyncRun make && ./a0 < sphere.obj<CR>
nnoremap <F6> :w<cr>:AsyncRun make && ./a0 < garg.obj<CR>
