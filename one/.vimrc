command! MakeTags :AsyncRun ctags -f .tags -R . --c++-kinds=+p

" .gl_tags: generated from `ctags -f .gl_tags --c++-kinds=+p  -R /usr/include/GL/`
set tags+=.gl_tags

nnoremap <F4> :w<cr>:AsyncRun make<cr>
nnoremap <F5> :w<cr>:AsyncRun make && ./a1 swp/core.swp<CR>
" nnoremap <F6> :w<cr>:AsyncRun make && ./a0 < garg.obj<CR>
