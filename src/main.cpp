#include "./include/main.h"
#include "./include/game_of_life.h"

enum MESSAGE_TAG {
  MATRIX_DATA = 0,
  MATRIX_SIZE = 1,
  ROWS_TO_UPDATE = 2,
  ROW_BEGIN = 3,
  CONTROL = 4
};

enum STATE {
  WORK = 0,
  EXIT = 1
};

void sendMatrixMPI(const std::vector<std::vector<uint8_t>> &matrix, int32_t destiny) {
  auto matrix_rows = matrix.size();
  auto matrix_cols = matrix[0].size();
  
  MPI_Send(&matrix_rows, 1, MPI_UINT64_T, destiny, MATRIX_SIZE, MPI_COMM_WORLD);
  MPI_Send(&matrix_cols, 1, MPI_UINT64_T, destiny, MATRIX_SIZE, MPI_COMM_WORLD);

  for (size_t i = 0; i < matrix.size(); ++i) {
    MPI_Send(&matrix[i][0], matrix[i].size(), MPI_UINT8_T, destiny, MATRIX_DATA, MPI_COMM_WORLD);
  }
}

std::vector<std::vector<uint8_t>> receiveMatrixMPI(int32_t source) {
  uint64_t matrix_rows = 0;
  uint64_t matrix_cols = 0;

  MPI_Recv(&matrix_rows, 1, MPI_UINT64_T, source, MATRIX_SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  MPI_Recv(&matrix_cols, 1, MPI_UINT64_T, source, MATRIX_SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  std::vector<std::vector<uint8_t>> matrix = std::vector<std::vector<uint8_t>>(matrix_rows, std::vector<uint8_t>(matrix_cols));


  for (size_t i = 0; i < matrix.size(); ++i) {
    MPI_Recv(&matrix[i][0], matrix[i].size(), MPI_UINT8_T, source, MATRIX_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  return matrix;
}

int32_t main(int argc, char* argv[]) {
  int32_t worldSize = 0;
  int32_t worldRank = 0;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);


  if (worldRank == 0) {
    auto matrix = create_grid(worldSize, worldSize);
    matrix[0][1] = 1;
    matrix[1][2] = 1;
    matrix[2][0] = 1;
    matrix[2][1] = 1;
    matrix[2][2] = 1;

    print_matrix(matrix);

    for(size_t iteration = 0; iteration < 5; ++iteration)
    { 
      // Distribute work
      distributeWork(matrix, worldSize);
      
      // Do work on this process
      auto [rows, row_begin] = update_rows(matrix, 0, 1);
      for (auto c = row_begin; c < row_begin + rows.size(); ++c) {
        matrix[c] = rows[c - row_begin];
      }

      // Gather results and print
      gatherResults(matrix, worldSize);
      print_matrix(matrix);

      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    // Send control message to exit
    for (int32_t i = 1; i < worldSize; ++i) {
      auto value = (uint64_t) STATE::EXIT;
      MPI_Send(&value, 1, MPI_UINT64_T, i, MESSAGE_TAG::CONTROL, MPI_COMM_WORLD);
    }

  }
  else {
    while (true){
      uint64_t control_message = 0;
      MPI_Recv(&control_message, 1, MPI_UINT64_T, 0, MESSAGE_TAG::CONTROL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if (control_message == STATE::EXIT) {
        break;
      }
      auto matrix = receiveMatrixMPI(0);

      size_t row_begin = 0;
      size_t rows_to_update = 0;
      MPI_Recv(&row_begin, 1, MPI_UINT64_T, 0, MESSAGE_TAG::ROW_BEGIN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(&rows_to_update, 1, MPI_UINT64_T, 0, MESSAGE_TAG::ROWS_TO_UPDATE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      auto [updated_rows, row_started] = update_rows(matrix, row_begin, rows_to_update); 

      sendMatrixMPI(updated_rows, 0);

      MPI_Send(&row_begin, 1, MPI_UINT64_T, 0, MESSAGE_TAG::ROW_BEGIN, MPI_COMM_WORLD);
    }
  }
  MPI_Finalize();
  return EXIT_SUCCESS;
}

void distributeWork(const std::vector<std::vector<uint8_t>> &matrix, int32_t worldSize) {
  for (int32_t i = 1; i < worldSize; ++i) {
      // Send control message
      auto value = (uint64_t) STATE::WORK;
      MPI_Send(&value, 1, MPI_UINT64_T, i, MESSAGE_TAG::CONTROL, MPI_COMM_WORLD);

      sendMatrixMPI(matrix, i);
    
      auto row_start = (size_t) i;
      auto rows_to_update = (size_t) 1;

      // Send row start
      MPI_Send(&row_start, 1, MPI_UINT64_T, i, MESSAGE_TAG::ROW_BEGIN, MPI_COMM_WORLD);
      // Send number of rows
      MPI_Send(&rows_to_update, 1, MPI_UINT64_T, i, MESSAGE_TAG::ROWS_TO_UPDATE, MPI_COMM_WORLD);
    }
}

void gatherResults(std::vector<std::vector<uint8_t>> &matrix, int32_t worldSize) {
  for (int32_t i = 1; i < worldSize; ++i){
    auto updated_rows = receiveMatrixMPI(i);
    size_t row_begin = 0;
    MPI_Recv(&row_begin, 1, MPI_UINT64_T, i, MESSAGE_TAG::ROW_BEGIN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (auto c = row_begin; c < row_begin + updated_rows.size(); ++c) {
      matrix[c] = updated_rows[c - row_begin];
    }
  }
}

void print_matrix(const std::vector<std::vector<uint8_t>> &matrix) {
  for (auto row : matrix) {
    for (auto cell : row) {
      std::cout << static_cast<int32_t>(cell) << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}