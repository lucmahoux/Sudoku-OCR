#ifndef DATA_SET_H
#define DATA_SET_H

#include <stdint.h>
#include <stdio.h>

// Making sure that the file is read properly
// Comes with the dataset found on the internet
// See  Yann Lecun's website for more precision
#define LABEL_MAGIC 2049
#define IMAGE_MAGIC 2051

// MNSIT Dataset is made of image of 28 by 28 pixels

#define IMAGE_WIDTH 28
#define IMAGE_HEIGHT 28
#define IMAGE_SIZE IMAGE_WIDTH * IMAGE_HEIGHT

// Each image of the dataset is labelled wit a number from 0 to 9
// 10 labels total

#define LABELS 10

// Each file of the MNSIT dataset contains a few informations about itself
// Including the number of labels
// I use uint32_t beacause I need to be sure that there are 32 bits assigned
// __attribute__((packed)) is to make sure that the minimal memory space is used
// the field magic_number will be tested to make sure that the file is read properly


typedef struct label_file_header {
    uint32_t magic_number;
    uint32_t number_of_labels;
} __attribute__((packed)) label_file_header;

typedef struct image_file_header {
    uint32_t magic_number;
    uint32_t number_of_images;
    uint32_t number_of_rows;
    uint32_t number_of_columns;
} __attribute__((packed)) image_file_header;

typedef struct Image {
    uint8_t pixels[IMAGE_SIZE];
} __attribute__((packed)) Image;

// struct of the dataset
typedef struct Dataset {
    Image * images;
    uint8_t * labels;
    uint32_t size;
} Dataset;

Dataset *dataset_load_csv(char* filename,size_t nb_rows, size_t nb_columns);
Dataset * get_dataset(char * image_path, char * label_path);
void free_dataset(Dataset * dataset);
Image *create_image(size_t size);

#endif
