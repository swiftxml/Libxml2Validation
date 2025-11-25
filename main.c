/**
 * section: xmlReader
 * synopsis: Parse and validate an XML file with an xmlReader
 * purpose: Demonstrate the use of xmlReaderForFile() to parse an XML file
 *          validating the content in the process and activating options
 *          like entities substitution, and DTD attributes defaulting.
 *          (Note that the XMLReader functions require libxml2 version later
 *          than 2.6.)
 * usage: reader2 <valid_xml_filename>
 * test: reader2 test2.xml > reader1.tmp && diff reader1.tmp $(srcdir)/reader1.res
 * author: Daniel Veillard
 * copy: see Copyright for the status of this software.
 */

#include <stdio.h>
#include <libxml/xmlreader.h>
#include <string.h>

#ifdef LIBXML_READER_ENABLED

int debug = 0;

/**
 * processNode:
 * @reader: the xmlReader
 *
 * Dump information about the current node
 */
static void
processNode(xmlTextReaderPtr reader) {
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);
    if (name == NULL)
        name = BAD_CAST "--";

    value = xmlTextReaderConstValue(reader);

    printf("%d %d %s %d %d",
           xmlTextReaderDepth(reader),
           xmlTextReaderNodeType(reader),
           name,
           xmlTextReaderIsEmptyElement(reader),
           xmlTextReaderHasValue(reader));
    if (value == NULL)
        printf("\n");
    else {
        if (xmlStrlen(value) > 40)
            printf(" %.40s...\n", value);
        else
            printf(" %s\n", value);
    }
}

/**
 * streamFile:
 * @filename: the file name to parse
 *
 * Parse, validate and print information about an XML file.
 */
static void
streamFile(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;


    /*
     * Pass some special parsing options to activate DTD attribute defaulting,
     * entities substitution and DTD validation
     */
    reader = xmlReaderForFile(filename, NULL,
                              XML_PARSE_DTDATTR |  /* default DTD attributes */
                              XML_PARSE_NOENT |    /* substitute entities */
                              XML_PARSE_DTDVALID); /* validate with the DTD */
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            //processNode(reader);
            ret = xmlTextReaderRead(reader);
        }
        /*
         * Once the document has been fully parsed check the validation results
         */
        if (xmlTextReaderIsValid(reader) != 1) {
            fprintf(stderr, "Document %s does not validate\n", filename);
        }
        printf("[ENCODING=%s]\n",xmlTextReaderConstEncoding(reader));
        xmlFreeTextReader(reader);
        if (ret != 0) {
            fprintf(stderr, "%s : failed to parse\n", filename);
        }
    } else {
        fprintf(stderr, "Unable to open %s\n", filename);
    }
}

int startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
            lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}

#if defined (_WIN32)
int setenv(const char *name, const char *value, int overwrite)
{
    return _putenv_s(name, value);
}

int unsetenv(const char *name)
{
    return setenv(name, "", 1);
}
#endif

int main(int argc, char **argv) {

    char* doc = NULL;
    char* catalog = NULL;

    for (int i=1; i<argc; i++) {
        char* arg = argv[i];

        if (startsWith("-catalog=", arg)) {
            catalog = arg + strlen("-catalog=");
        }
        else if (strcmp(arg, "-debug") == 0) {
            debug = 1;
        }
        else {
            doc = arg;
        }
    }

    if (doc == NULL) {
        fprintf(stderr, "no documment provided\n");
        printf("usage: %s <document> [-catalog=<catalog>] [-debug]\n", argv[0]);
        return (1);
    }

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    if (debug) {
        printf("document: [%s]\n", doc);
    }

    if (catalog != NULL) {
        if (debug) {
            printf("catalog: [%s]\n", catalog);
            setenv("XML_DEBUG_CATALOG", "YES", 1);
        }
        setenv("XML_CATALOG_FILES", catalog, 1);
    }
    // out-commented: do _not_ unset because XML_CATALOG_FILES might be set from the outside:
    /*else {
        unsetenv("XML_CATALOG_FILES");
    }*/

    if (debug) {
        printf("[XML_DEBUG_CATALOG=%s]\n", getenv("XML_DEBUG_CATALOG"));
        printf("[XML_CATALOG_FILES=%s]\n", getenv("XML_CATALOG_FILES"));
    }

    streamFile(doc);

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
    return(0);
}

#else
int main(void) {
    fprintf(stderr, "XInclude support not compiled in\n");
    exit(1);
}
#endif