#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <xml.h>


/**
 * * Will halt the program iff assertion fails
 * */
static void _assert_that(_Bool condition, char const* message, char const* func, char const* file, int line) {
    if (!condition) {
        fprintf(stderr, "Assertion failed: %s, in %s (%s:%i)\n", message, func, file, line);
        exit(EXIT_FAILURE);
    }
}

#define assert_that(condition, message) \
    _assert_that(condition, message, __func__, __FILE__, __LINE__)



int main(int argc, char** argv) {
#define FILE_NAME "test.xml"
    FILE* handle = fopen(FILE_NAME, "rb");
    assert_that(handle, "Cannot open " FILE_NAME);

    struct xml_document* document = xml_open_document(handle);
    assert_that(document, "Cannot parse " FILE_NAME);

    struct xml_node* element = xml_easy_child(
            xml_document_root(document), "Element", "With", 0
            );
    assert_that(element, "Cannot find Document/Element/With");


    fprintf(stderr, "HI! Element content is : %s", xml_easy_content(element));

    xml_document_free(document, true);
#undef FILE_NAME
}
