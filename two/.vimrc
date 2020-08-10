command! MakeTags :AsyncRun ctags -f .tags -R . --c++-kinds=+p

set path+=../vecmath/include

" .gl_tags: generated from `ctags -f .gl_tags --c++-kinds=+p  -R /usr/include/GL/`
set tags+=.gl_tags

" let $ARGS = "swp/core.swp"
" let $MYARGS = "swp/mycore.swp"
" let $DEBUG = 1
" nnoremap <F4> :AsyncRun -save=1 -auto= make DEBUG=$DEBUG && echo "DEBUG="$DEBUG<cr>
" nnoremap <F5> :AsyncRun -save=1 -program=make -auto= DEBUG=$DEBUG && ./a1 $ARGS<cr>
" nnoremap <F6> :AsyncRun -save=1 -program=make -auto= DEBUG=$DEBUG && ./a1 $MYARGS<cr>
