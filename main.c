#include <stdio.h>
#include <mupdf/fitz.h>

void printOutline(fz_context *ctx, fz_outline *outline, int level)
{
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    printf("%s\n", outline->title);

    if (outline->down != NULL) {
        printOutline(ctx, outline->down, level + 1);
    }

    if (outline->next != NULL) {
        printOutline(ctx, outline->next, level);
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

    printOutline(ctx, outline, 0);

    fz_drop_outline(ctx, outline);
    fz_drop_document(ctx, doc);
    fz_drop_context(ctx);

    return 0;
}