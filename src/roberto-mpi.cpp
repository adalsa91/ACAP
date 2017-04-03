#include <CImg.h>
#include <iostream>
#include <mpi.h>

using namespace cimg_library;

void MPI_Send_image(CImg<double> img, int dest, int tag, MPI_Comm comm) {
    int img_size[3] = { img.width(), img.height(), img.spectrum() };
    
    MPI_Send(&img_size, 3, MPI_INT, dest, 0, comm);
    
    MPI_Send(img.data(), img_size[0] * img_size[1] * img_size[2], MPI_DOUBLE, dest, tag, comm);

}

CImg<double> MPI_Recv_image(int source, int tag, MPI_Comm comm, MPI_Status *status) {
    int img_size[3];
    MPI_Recv(&img_size, 3, MPI_INT, source, 0, comm, status);

    CImg<double> img(img_size[0], img_size[1], 1, img_size[2], 0);

    MPI_Recv(img.data(), img_size[0] * img_size[1] * img_size[2], MPI_DOUBLE, source, tag, comm, status);

    return img;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "You need to supply a image to this program." << std::endl;
        return -1;
    }

    int nproc, rank;

    /* Initializes the MPI execution environment */
    MPI_Init(&argc, &argv);
    /* Returns the size of the group MPI_COMM_WORLD */
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    /* Return the rank of the calling process in the MPI_COMM_WORLD group */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    //Work-sharing tasks
    if (rank == 0) {
        //Load original image
        CImg<double> original_image(argv[1]);

        //Crop image vertically to distribute work
        int width_slice = original_image.width() / (nproc-1);
       
        for (int i = 0; i < (nproc-1); i++) {
            CImg<double> cropped_image = original_image.get_crop(
                                            i * width_slice,
                                            0,
                                            ((i+1) * width_slice)-1,
                                            original_image.height()-1
                                        );
            MPI_Send_image(cropped_image, i+1 , 2, MPI_COMM_WORLD);
        }

        //Collect and join cropped images
        CImg<double> result = MPI_Recv_image(1, 1, MPI_COMM_WORLD, &status);
        for (int i=2; i<nproc; i++) {
            result.append(MPI_Recv_image(i, 1, MPI_COMM_WORLD, &status));
        } 

        result.save("result.png");

    } else {

        CImg<double> cropped_image = MPI_Recv_image(0, 2, MPI_COMM_WORLD, &status);

        //Create gradient kernel for Roberts operator
        CImg<> gradient_x = CImg<>(3,3).fill(
            0, 0, 0,
            0, 1, 0,
            0, 0, -1
        );
        CImg<> gradient_y = CImg<>(3,3).fill(
            0, 0, 0,
            0, 0, 1,
            0, -1, 0
        );
        
        //Time to work
        
        cropped_image = (cropped_image.get_convolve(gradient_x).abs() + cropped_image.get_convolve(gradient_y).abs()).normalize(0,255);

        MPI_Send_image(cropped_image, 0, 1, MPI_COMM_WORLD);

    }

    /* Terminates MPI execution environment.*/
    MPI_Finalize();
    return(0);    
}