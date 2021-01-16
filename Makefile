# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pmetron <pmetron@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/11/07 15:39:13 by hunnamab          #+#    #+#              #
#    Updated: 2021/01/16 19:34:02 by pmetron          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = rt
NAME_LINUX = rt_linux
LIB_FLAGS = -Wall -Wextra
MAC_FLAGS = -I SDL2.framework/Headers -F ./ -framework SDL2 -framework OpenCL -I SDL2_image.framework/Headers -framework SDL2_image -I SDL2_mixer.framework/Headers -framework SDL2_mixer
LINUX_FLAGS = -lSDL2 -lm -lXext -lcuda -lcudart
LIBRARY = ./libft/libft.a 
INCLUDE = ./includes/
SRC = main.c sphere.c vector.c utils.c \
	light.c triangle.c scenes_reader.c draw.c \
	objects_parameters.c plane.c cylinder.c cone.c \
	./matrix_lib/matr_add_matr.c ./matrix_lib/create_matrix.c \
	./matrix_lib/matr_copy.c ./matrix_lib/matr_div_by_scalar.c \
	./matrix_lib/matr_free.c ./matrix_lib/matr_mul_by_scalar.c \
	./matrix_lib/matr_mul.c  ./matrix_lib/matr_sub_matr.c \
	./matrix_lib/matr_sub_scalar.c ./matrix_lib/matr_to_line.c \
	./matrix_lib/matr_trace.c ./matrix_lib/matr_transpose.c \
	transform.c ./matrix_lib/matrix_identity.c \
	buffers.c scene.c color.c vector_second.c transform_matrix.c \
	keyboard.c clean.c errors_management.c light_parameters.c \
	parameters_utils.c camera_parameters.c define_object.c \
	scenes_reader_util.c cl_init.c textures.c texture_mapping.c \
	buffers_material_buf.c texture_loading.c ellipsoid.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(OBJ): %.o: %.c $(INCLUDE)
		gcc -c $(LIB_FLAGS) -I libft/ -I matrix_lib/ -o $@ $< -I $(INCLUDE)

$(LIBRARY):
		@make -C libft/

$(NAME): $(LIBRARY) $(OBJ) $(INCLUDE)
		@cp -r SDL2.framework ~/Library/Frameworks/
		@cp -r SDL2_image.framework ~/Library/Frameworks/
		@cp -r SDL2_mixer.framework ~/Library/Frameworks/
		@gcc $(OBJ) $(LIBRARY) -o $(NAME) $(MAC_FLAGS) -I $(INCLUDE)

clean:
	@rm -f $(OBJ)
	@make -C libft clean
fclean: clean
	@rm -f $(NAME)
	@rm -rf ~/Library/Frameworks/SDL2.framework
	@rm -rf ~/Library/Frameworks/SDL2_image.framework
	@rm -rf ~/Library/Frameworks/SDL2_mixer.framework
	@make -C libft fclean

re: fclean all

linux: $(NAME_LINUX)

$(NAME_LINUX): $(OBJ_LINUX)
	nvcc -c ./linux_src/*.cu ./linux_src/*.c ./matrix_lib/*.c -lSDL2 -lm -lXext -lcuda -lcudart -I ./linux_src/headers/
	nvcc *.o $(LIBRARY) -o $(NAME_LINUX) $(LINUX_FLAGS)
