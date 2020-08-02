command! MakeTags :AsyncRun ctags -f .tags -R . --c++-kinds=+p

set path+=../vecmath/include

" .gl_tags: generated from `ctags -f .gl_tags --c++-kinds=+p  -R /usr/include/GL/`
set tags+=.gl_tags

nnoremap <F4> :w<cr>:AsyncRun make DEBUG=1<cr>
nnoremap <F5> :w<cr>:AsyncRun make DEBUG=1 && ./a1 swp/core.swp<CR>
nnoremap <F6> :w<cr>:AsyncRun make DEBUG=1 && ./a1 swp/mycore.swp<CR>
" nnoremap <F6> :w<cr>:AsyncRun make && ./a0 < garg.obj<CR>
