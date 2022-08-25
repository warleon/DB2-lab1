#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace P4 {

typedef struct {
  int ciclo;
  float mensualidad;
  std::string codigo;
  std::string observaciones;
} Matricula;
typedef struct {
  int g;
  int size;
  int fieldSize[2];
} Metadata;

class Database {
  static const auto bin = std::fstream::binary;
  static const auto in = std::fstream::in;
  static const auto out = std::fstream::out;
  static const auto app = std::fstream::app;

  std::string filename;
  std::string metaname;

  Matricula parse(Metadata met, char* data) {
    Matricula mat;
    memcpy((char*)&mat, data, sizeof(int) + sizeof(float));
    mat.codigo = std::move(
        std::string(data + offsetof(Matricula, codigo),
                    data + offsetof(Matricula, codigo) + met.fieldSize[0]));
    mat.codigo = std::move(
        std::string(data + offsetof(Matricula, codigo) + met.fieldSize[0],
                    data + offsetof(Matricula, codigo) + met.fieldSize[0] +
                        met.fieldSize[1]));
    return mat;
  }
  char* dump(Metadata met, Matricula data) {
    char* buff = new char[met.size];
    memcpy(buff, &data.ciclo, sizeof(int));
    memcpy(buff + offsetof(Matricula, mensualidad), &data.mensualidad,
           sizeof(float));
    memcpy(buff + offsetof(Matricula, codigo), data.codigo.c_str(),
           met.fieldSize[0]);
    memcpy(buff + offsetof(Matricula, codigo) + met.fieldSize[0],
           data.observaciones.c_str(), met.fieldSize[1]);
    return buff;
  }
  Metadata getSomeMeta(Matricula matri) {
    Metadata meta;
    meta.fieldSize[0] = matri.codigo.size();
    meta.fieldSize[1] = matri.observaciones.size();
    meta.size =
        sizeof(int) + sizeof(float) + meta.fieldSize[0] + meta.fieldSize[1];
    return meta;
  }

 public:
  enum filetype : char { data = 0, meta };

  Database(std::string filename_)
      : filename(filename_), metaname(filename_ + ".meta") {}

  int binSize(filetype type) {
    std::ofstream file;
    switch (type) {
      case filetype::data:
        file.open(filename, bin);
        break;
      case filetype::meta:
        file.open(metaname, bin);
        break;
      default:
        break;
    }
    if (!file.good()) throw std::runtime_error("error oppening file at size\n");
    file.seekp(0, std::ios::end);
    return file.tellp();
  }

  std::vector<Matricula> load() {
    std::ifstream file(filename, bin);
    if (!file.good())
      throw std::runtime_error("error oppening datafile at load\n");
    std::ifstream metafile(metaname, bin);
    if (!file.good())
      throw std::runtime_error("error oppening metafile at load\n");
    int n = binSize(filetype::meta) / sizeof(Metadata);
    std::vector<Matricula> records(n);
    Metadata meta = {};
    Matricula mat = {};
    for (int pos = 0; pos < n; pos++) {
      metafile.seekg(pos * sizeof(Metadata));
      metafile.read((char*)&meta, sizeof(Metadata));
      char* data = new char[meta.size];
      file.read(data, meta.size);
      mat = parse(meta, data);
      delete[] data;
      records[pos] = mat;
    }
    return records;
  }

  void add(Matricula record) {
    std::ofstream file(filename, bin);
    if (!file.good())
      throw std::runtime_error("error oppening datafile at add\n");
    file.seekp(0, std::ios::end);
    std::ofstream metafile(metaname, bin | app);
    if (!file.good())
      throw std::runtime_error("error oppening metafile at add\n");
    Metadata metaRecord = getSomeMeta(record);
    metaRecord.g = file.tellp();
    metafile.write((char*)&metaRecord, sizeof(Metadata));
    if (!file.good())
      throw std::runtime_error("error writing metafile at add\n");
    metafile.close();
    auto data = dump(metaRecord, record);
    file.write(data, metaRecord.size);
    if (!file.good())
      throw std::runtime_error("error writing datafile at add\n");
    file.close();
    delete[] data;
  }

  Matricula readRecord(int pos) {
    std::ifstream file(filename, bin);
    if (!file.good())
      throw std::runtime_error("error oppening datafile at readRecord\n");
    std::ifstream metafile(metaname, bin);
    if (!file.good())
      throw std::runtime_error("error oppening metafile at readRecord\n");

    metafile.seekg(pos * sizeof(Metadata));
    Metadata meta = {};
    metafile.read((char*)&meta, sizeof(Metadata));
    char* data = new char[meta.size];
    file.read(data, meta.size);
    Matricula mat = parse(meta, data);
    delete[] data;
    return mat;
  }
};

int tests() {
  int errors = 0;
  Database db("p4test");
  return errors;
}
}  // namespace P4