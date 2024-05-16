FROM archlinux

WORKDIR /app

RUN pacman --noconfirm -Syu 

RUN pacman --noconfirm -S tcc make 

COPY . . 

ENV ADDRESS_BOOK_PATH="./example.babook.csv"

RUN make

CMD ["./bin/babook","-s"]

