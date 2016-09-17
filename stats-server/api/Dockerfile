FROM golang

ADD . /go/src/github.com/tivvit/smart-home-api
ADD *.json /config/

RUN go get -u github.com/tools/godep \
    && cd /go/src/github.com/tivvit/smart-home-api \
    && godep get

RUN go install github.com/tivvit/smart-home-api
    
RUN rm -rf $GOPATH/src/* \
    && rm -rf $GOPATH/pkg/*

ENTRYPOINT /go/bin/smart-home-api

EXPOSE 8000
