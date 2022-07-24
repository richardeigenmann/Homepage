# Richi's Homepage

<http://richieigenmann.users.sourceforge.net/>

```bash
su
JAVA_HOME=/usr/java/zulu17.32.13-ca-jdk17.0.2-linux_x64/
ant pushSourceforge
docker-compose up -d --build

rsync -v --progress \
--exclude bower_components \
--exclude nbproject \
--exclude weblog \
--exclude .gitignore \
--exclude bower.json \
--exclude build.xml \
--exclude docker-compose.yml \
--exclude Dockerfile \
--exclude generateComputerbooks.json.php \
--exclude Readme.md \
--exclude Template.htm \
/richi/Src/Homepage/* richieigenmann@frs.sourceforge.net:/home/user-web/richieigenmann/htdocs/
```
