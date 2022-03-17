%include "lib.inc"
%include "dict.inc"
%include "colon.inc"
%include "words.inc"

global _start

section .rodata
notFoundErr: db "Input string not found:(", 0
overErr: db "Input is too long!", 0

section .text

_start:
	sub rsp, 256		;Выделяем место под буффер
	mov rdi, rsp		;Кладем адрес начала буффера
	mov rsi, 256		;Кладем размер буфера 
	call read_word		;Считываем ввод
	cmp rax, 0		;Проверяем влез ли ввод в буфер
	je .overflow		;Если нет(возвращен 0), то переход
	mov rdi, rax		;Кладем адрес буффера
	mov rsi, ARG		;Кладем указатель на строку
	call find_word		;Ищем строку
	cmp rax, 0		;Проверяем найдена ли строка
	je .not_found		;Если нет(возвращен 0), то переход
	add rax, 8		;Сдвигаем указатель на ключ
	push rax		;Сохраняем указатель на ключ
	mov rdi, rax		;Кладем указатель на ключ
	call string_length	;Находим длину строки
	inc rax		;Увеличиваем длину (под нуль-терминатор)
	pop rdi		;Достаем указатель на ключ
	add rdi, rax		;Смещаем указатель на значение
	call print_string	;Печатаем строку
	jmp .finish		;Переход
.not_found:
	mov rdi, notFoundErr	;Кладем указатель на нужное сообщение об ошибке
	call print_error	;Выводим сообщение об ошибке
	jmp .finish		;Переход
.overflow:
	mov rdi, overErr	;Кладем указатель на нужное сообщение об ошибке
	call print_error	;Выводим сообщение об ошибке
.finish:
	call print_newline	;Переходим на новую строку
	add rsp, 256		;Возвращаем указатель на стек в исходное положение
	mov rax, 0		;Очищаем rax
	call exit		;Выходим

