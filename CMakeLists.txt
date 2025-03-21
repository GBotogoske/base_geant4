cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(Geant4AlphaSimulation)

# 1️⃣ Encontrar o Geant4 instalado na máquina (vai procurar paths e libs)
# Aqui você informa explicitamente o caminho correto para o Geant4 instalado manualmente:
set(Geant4_DIR /home/gabriel/Documents/geant4/v11.2.2/geant4-v11.2.2-install/lib/cmake/Geant4)
find_package(Geant4 REQUIRED ui_all vis_all)

# 2️⃣ Incluir diretórios onde estão headers do Geant4 e do seu código
include_directories(${Geant4_INCLUDE_DIRS} include)

# 3️⃣ Procurar automaticamente todos os arquivos .cc e .hh nas pastas src/ e include/
file(GLOB main_source ${PROJECT_SOURCE_DIR}/main.cc)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# 4️⃣ Criar o executável chamado "geant4_alpha_sim" juntando todas as fontes e headers
add_executable(geant4_alpha_sim ${sources} ${headers} ${main_source})

# 5️⃣ Ligar seu executável às bibliotecas do Geant4 encontradas
target_link_libraries(geant4_alpha_sim ${Geant4_LIBRARIES})

# 6️⃣ Definir saída do executável diretamente na pasta principal do projeto
set_target_properties(geant4_alpha_sim PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# 7️⃣ Instalar macros se desejar
install(FILES init_vis.mac run.mac DESTINATION share/Geant4AlphaSimulation)

# ✅ Agora o executável será gerado na pasta principal do projeto após make

# ✅ Coloque esse arquivo na pasta raiz do projeto e use:
# mkdir build && cd build
# cmake ..
# make -j$(nproc)
