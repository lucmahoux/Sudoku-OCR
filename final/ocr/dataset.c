#include "dataset.h"
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// On some processors the value may need to be shifted
// Better safe than sorry
// See Yann Lecun's website for more informations

uint32_t map_uint32(uint32_t in)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (((in & 0xFF000000) >> 24) | ((in & 0x00FF0000) >> 8)
            | ((in & 0x0000FF00) << 8) | ((in & 0x000000FF) << 24));
#else
    return in;
#endif
}

// read the label files
// take the path of the files
// modify number_of_labels by reference

uint8_t *get_labels( char *path, uint32_t *number_of_labels)
{
    FILE *stream;
    label_file_header header;
    uint8_t *labels;

    stream = fopen(path, "rb");

    size_t num_header =  fread(&header, sizeof(label_file_header), 1, stream);

    header.magic_number = map_uint32(header.magic_number);
    header.number_of_labels = map_uint32(header.number_of_labels);
    *number_of_labels = header.number_of_labels;


    labels = malloc(*number_of_labels * sizeof(uint8_t));

    size_t num_labels = fread(labels, 1, *number_of_labels, stream);
    if(*number_of_labels != num_labels || num_header != 1 || header.magic_number != 2049)
        errx(1, "Not  able to read labels");
    fclose(stream);

    return labels;
}

/**
 * Read images from file.
 *
 * File format: http://yann.lecun.com/exdb/mnist/
 */
Image *get_images( char *path, uint32_t *number_of_images)
{
    FILE *stream;
    image_file_header header;
    Image *images;

    stream = fopen(path, "rb");

    size_t num_header = fread(&header, sizeof(image_file_header), 1, stream);
    header.magic_number = map_uint32(header.magic_number);
    header.number_of_images = map_uint32(header.number_of_images);
    header.number_of_rows = map_uint32(header.number_of_rows);
    header.number_of_columns = map_uint32(header.number_of_columns);

    *number_of_images = header.number_of_images;
    images = malloc(*number_of_images * sizeof(Image));

    size_t num_images = fread(images, sizeof(Image), *number_of_images, stream);
    if (*number_of_images != num_images || num_header != 1 || header.magic_number != 2051)
        err(1, "Not able to read images");


    fclose(stream);

    return images;
}

Dataset *get_dataset( char *image_path,  char *label_path)
{
    Dataset *dataset;
    uint32_t number_of_images, number_of_labels;

    dataset = malloc(sizeof(Dataset));

    dataset->images = get_images(image_path, &number_of_images);

    dataset->labels = get_labels(label_path, &number_of_labels);

    dataset->size = number_of_images;

    return dataset;
}

char** read_line_csv(FILE* file, size_t nb_columns)
{
	char line[4096];
	if (file == NULL || NULL == fgets(line, 4096, file)){
		return NULL;
	};
	char** parsed_line = malloc((nb_columns + 1) * sizeof(char*));
	parsed_line[nb_columns] = NULL;
	size_t j = 0;
	char* buf = calloc(64, sizeof(char));
	size_t k = 0;
	int in_parenthesis = 0;
	for(size_t i = 0; line[i] != '\n' && line[i] != '\0'; ++i){
		if(line[i] == '['){
			in_parenthesis = 1;
			continue;
		}
		if(line[i] == ']'){
			in_parenthesis = 0;
			continue;
		}
		if(line[i] == ',' && !in_parenthesis){
			buf[k] = '\0';
			k = 0;
			parsed_line[j] = buf;
			buf = calloc(64, sizeof(char));
			++j;
		}
		else {
			buf[k] = line[i];
			++k;
		}
	}
	buf[k] = '\0';
	parsed_line[j] = buf;
	if(j != nb_columns - 1) errx(1, "Not enough columns in CSV file");
	return parsed_line;
}


void free_parsed_line(char** parsed_line){
	if(parsed_line == NULL) return;
	for(size_t i = 0; parsed_line[i] != NULL; ++i) free(parsed_line[i]);
	free(parsed_line);
}


Dataset *dataset_load_csv(char* filename,size_t nb_rows,
								   size_t nb_columns){
	FILE* file = fopen(filename, "r");
	Dataset *csv_data = malloc(sizeof(Dataset));
    csv_data->images = malloc((nb_rows)*sizeof(Image));
    csv_data->labels = malloc((nb_rows)*sizeof(uint8_t));
	for(size_t i = 0; i < nb_rows; ++i)
    {

		char** parsed_line = read_line_csv(file, nb_columns);
		if (parsed_line == NULL)
            return csv_data;
		csv_data->labels[i] = (uint8_t)atoi(parsed_line[0]);
		// We hope it's a square, we trust the user
		Image *image = malloc(sizeof(Image));
		for(size_t j = 1; j < nb_columns; ++j)
    		image->pixels[j - 1] = (float)atoi(parsed_line[j]);
        csv_data->images[i] = *image;
        free(image);
		free_parsed_line(parsed_line);
	}

	fclose(file);
	csv_data->size = nb_rows - 1;
	return csv_data;
}

/**
 * Free all the memory allocated in a dataset. This should not be used on a
 */
void free_dataset(Dataset *dataset)
{
    free(dataset->images);
    free(dataset->labels);
    free(dataset);
}

Image *create_images(size_t size)
{
    Image *image = malloc(size*sizeof(Image));
    return image;
}
