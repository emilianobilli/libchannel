/*

func main() {
	type Book struct{id int}
	bookshelf := make(chan Book, 3)

	for i := 0; i < cap(bookshelf) * 2; i++ {
		select {
		case bookshelf <- Book{id: i}:
			fmt.Println("succeeded to put book", i)
		default:
			fmt.Println("failed to put book")
		}
	}

	for i := 0; i < cap(bookshelf) * 2; i++ {
		select {
		case book := <-bookshelf:
			fmt.Println("succeeded to get book", book.id)
		default:
			fmt.Println("failed to get book")
		}
	}
}

*/


#include <libchannel.h>
#include <stdio.h>

int main(void) {
    any_t book;
    int bookshelf;
    int i;
    int cap = 3;
    init_libchannel();
    
    bookshelf = make_chan(cap);

    for (i = 0; i < cap * 2; i++) {
        select_set_t op[] = {
            {bookshelf, OP_SEND, &book, NULL}
        };
        book.type = VAR_INT64;
        book.value.int64_val = i;
        if (select_chan(op, 1, SELECT_NONBLOCK) == bookshelf) {
            printf("success to put book %d\n", i);
        } else {
            printf("failed to put book\n");
        }        
    }

        for (i = 0; i < cap * 2; i++) {
        select_set_t op[] = {
            {bookshelf, OP_RECV, NULL, &book}
        };
        if (select_chan(op, 1, SELECT_NONBLOCK) == bookshelf) {
            printf("success to get book %lld\n", book.value.int64_val);
        } else {
            printf("failed to get book\n");
        }        
    }

}