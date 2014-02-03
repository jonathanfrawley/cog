/**
 * section: xmlReader
 * synopsis: Parse an XML file with an xmlReader
 * purpose: Demonstrate the use of xmlReaderForFile() to parse an XML file
 *          and dump the informations about the nodes found in the process.
 *          (Note that the XMLReader functions require libxml2 version later
 *          than 2.6.)
 * usage: reader1 <filename>
 * test: reader1 test2.xml > reader1.tmp && diff reader1.tmp $(srcdir)/reader1.res
 * author: Daniel Veillard
 * copy: see Copyright for the status of this software.
 */

#include <stdio.h>
#include <libxml2/libxml/xmlreader.h>

#include <cog_list.h>

typedef struct cog_tiled_image {
    char* source;
    int width;
    int height;
} cog_tiled_image;

typedef struct cog_tiled_tileset {
    int firstgid;
    char* name;
    int width;
    int height;
    cog_tiled_image image;
} cog_tiled_tileset;

typedef struct cog_tiled_property {
    char* name;
    char* value;
} cog_tiled_property;

typedef struct cog_tiled_tile {
    int gid;
} cog_tiled_tile;

typedef struct cog_tiled_data {
    cog_list tiles;
} cog_tiled_data;

typedef struct cog_tiled_object {
    char* name;
    int x;
    int y;
    int width;
    int height;
} cog_tiled_object;

typedef struct cog_tiled_objectgroup {
    char* name;
    int width;
    int height;
    cog_list objects;
} cog_tiled_objectgroup;

typedef struct cog_tiled_layer {
    cog_list properties;
    cog_tiled_data data;
    cog_list objectgroups;
} cog_tiled_layer;

typedef struct cog_tiled_map {
    int width;
    int height;
    int tile_width;
    int tile_height;
    cog_tiled_tileset tileset;
    cog_list layers;
} cog_tiled_map;

/**
 * processNode:
 * @reader: the xmlReader
 *
 * Dump information about the current node
 */
static void processNode(xmlTextReaderPtr reader) {
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);
    if (name == NULL)
    name = BAD_CAST "--";

    //printf("NAME : <%s> \n", name);
    if(xmlStrEqual("map", name)) {
        printf("Parsing map...\n");
        int attrib_cnt = xmlTextReaderAttributeCount(reader);
        //printf("Attrib cnt %d ", attrib_cnt);
        printf("width is <%s>\n", xmlTextReaderGetAttribute(reader, "width"));
        printf("height is <%s>\n", xmlTextReaderGetAttribute(reader, "height"));
        printf("tilewidth is <%s>\n", xmlTextReaderGetAttribute(reader, "tilewidth"));
        printf("tileheight is <%s>\n", xmlTextReaderGetAttribute(reader, "tileheight"));
    } else if(xmlStrEqual("tileset", name)) {
        printf("Parsing tileset...\n");
        printf("firstgid is <%s>\n", xmlTextReaderGetAttribute(reader, "firstgid"));
        printf("name is <%s>\n", xmlTextReaderGetAttribute(reader, "name"));
        printf("tilewidth is <%s>\n", xmlTextReaderGetAttribute(reader, "tilewidth"));
        printf("tileheight is <%s>\n", xmlTextReaderGetAttribute(reader, "tileheight"));
    } else if(xmlStrEqual("image", name)) {
        printf("Parsing image...\n");
        printf("source is <%s>\n", xmlTextReaderGetAttribute(reader, "source"));
        printf("width is <%s>\n", xmlTextReaderGetAttribute(reader, "width"));
        printf("height is <%s>\n", xmlTextReaderGetAttribute(reader, "height"));
    } else if(xmlStrEqual("layer", name)) {
        printf("Parsing layer...\n");
        printf("name is <%s>\n", xmlTextReaderGetAttribute(reader, "name"));
        printf("width is <%s>\n", xmlTextReaderGetAttribute(reader, "width"));
        printf("height is <%s>\n", xmlTextReaderGetAttribute(reader, "height"));
    } else if(xmlStrEqual("data", name)) {
        printf("Parsing data...\n");
        printf("data is <%s>\n", xmlTextReaderConstValue(reader));
        //TODO: All children
    } else if(xmlStrEqual("objectgroup", name)) {
        printf("Parsing objectgroup...\n");
        printf("name is <%s>\n", xmlTextReaderGetAttribute(reader, "name"));
        printf("width is <%s>\n", xmlTextReaderGetAttribute(reader, "width"));
        printf("height is <%s>\n", xmlTextReaderGetAttribute(reader, "height"));
    } else if(xmlStrEqual("object", name)) {
        printf("Parsing object...\n");
        printf("name is <%s>\n", xmlTextReaderGetAttribute(reader, "name"));
        printf("x is <%s>\n", xmlTextReaderGetAttribute(reader, "x"));
        printf("y is <%s>\n", xmlTextReaderGetAttribute(reader, "y"));
        printf("width is <%s>\n", xmlTextReaderGetAttribute(reader, "width"));
        printf("height is <%s>\n", xmlTextReaderGetAttribute(reader, "height"));
    }

    //value = xmlTextReaderConstValue(reader);
/*
    printf("%d %d %s %d %d", 
        xmlTextReaderDepth(reader),
        xmlTextReaderNodeType(reader),
        name,
        xmlTextReaderIsEmptyElement(reader),
        xmlTextReaderHasValue(reader));
    if(value == NULL) {
        printf("\n");
    } else {
        if (xmlStrlen(value) > 40)
            printf(" %.40s...\n", value);
        else
        printf(" %s\n", value);
    }
*/
}

/**
 * streamFile:
 * @filename: the file name to parse
 *
 * Parse and print information about an XML file.
 */
static void streamFile(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlReaderForFile(filename, NULL, 0);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            processNode(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            fprintf(stderr, "%s : failed to parse\n", filename);
        }
    } else {
        fprintf(stderr, "Unable to open %s\n", filename);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        return(1);
    }

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    streamFile(argv[1]);

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
