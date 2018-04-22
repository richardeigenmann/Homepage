# docker build -t richardeigenmann/richinet .
# docker run -it --rm --hostname richinet -v /richi/Src/RichiNet/weblog:/var/log/nginx/web -p8082:80 richardeigenmann/richinet

FROM nginx:1.13
RUN apt-get clean && apt-get update && apt-get install -y \
  php \
  php-mysqli \
  less vim

COPY vhost.conf /etc/nginx/conf.d/default.conf
COPY *htm *html *js *css *gif *png *jpg /var/www/
COPY Offline* /var/www/

COPY generateComputerbooks.json.php /var/www
WORKDIR /var/www

# to refresh the computerbooks.json
# docker exec -it homepage_web php /var/www/generateComputerbooks.json.php pw
# docker cp homepage_web:/var/www/computerbooks.json .

RUN ls -ltr /var/www

CMD nginx -g 'daemon off;'
