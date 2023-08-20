#include <stdio.h>
#include <mupdf/fitz.h>
#include <json-c/json.h>

void printOutline(fz_context *ctx, fz_outline *outline, int level, struct json_object* parent)
{
    struct json_object* titleObj = json_object_new_string(outline->title);
    json_object_array_add(parent, titleObj);

    if (outline->down != NULL) {
        struct json_object* childrenObj = json_object_new_array();
        json_object_array_add(parent, childrenObj);
        printOutline(ctx, outline->down, level + 1, childrenObj);
    }

    if (outline->next != NULL) {
        printOutline(ctx, outline->next, level, parent);
    }
}

int main(int argc, char *argv[])
{
    fz_context *ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);

    fz_try(ctx)
    {
        fz_register_document_handlers(ctx);
    }
    fz_catch(ctx)
    {
        printf("Failed to register document handlers: %s\n", fz_caught_message(ctx));
        fz_drop_context(ctx);
        return 1;
    }

    fz_document *doc = NULL;
    fz_try(ctx)
    {
        doc = fz_open_document(ctx, argv[1]);
    }
    fz_catch(ctx)
    {
        printf("Failed to open document: %s\n", fz_caught_message(ctx));
        fz_drop_context(ctx);
        return 1;
    }

    fz_outline *outline = NULL;
    fz_try(ctx)
    {
        outline = fz_load_outline(ctx, doc);
    }
    fz_catch(ctx)
    {
        printf("Failed to load outline: %s\n", fz_caught_message(ctx));
        fz_drop_document(ctx, doc);
        fz_drop_context(ctx);
        return 1;
    }

    struct json_object* rootObj = json_object_new_array();
    printOutline(ctx, outline, 0, rootObj);

    const char* jsonString = json_object_to_json_string_ext(rootObj, JSON_C_TO_STRING_PRETTY);
    printf("%s\n", jsonString);

    json_object_put(rootObj);

    fz_drop_outline(ctx, outline);
    fz_drop_document(ctx, doc);
    fz_drop_context(ctx);

    return 0;
}