## Requirements  
A mysql server on port 3306 (default) like [xampp](https://www.apachefriends.org/download.html)  
[ElasticSearch](https://www.elastic.co/downloads/elasticsearch?tech=rtp&pg=en-elasticsearch-page&plcmt=hero&cta=eswt-24510-a) on port 9200 (default)  

## Setup
1. Download the node packages  
`npm install`  
2. Create the `.env` file based on `.env.example `  
3. Generate the prisma client  
`npx prisma generate`  
4. Generate a database based on prisma  
`npx prisma db push` 

### ElasticSearch
&nbsp;&nbsp;&nbsp;&nbsp; Place [our config]((https://github.com/TanyasiPills/ColorSync/blob/main/docs/elasticsearch.yml)) (or create your own) into `\config\elastisearch.yml`

## Running
Start mysql  
Start ElasticSearch (run `bin\elasticsearch.bat` (or `bin/elasticsearch` on Linux))  
Start the server `npm run start`

## Example Database
You can download a example database from [main/Example Database/colorsync.sql](https://github.com/TanyasiPills/ColorSync/blob/main/Example%20Database/colorsync.sql).  
You can download the images for the example database from [main/Example Database/uplodas.zip](https://github.com/TanyasiPills/ColorSync/blob/main/Example%20Database/uploads.zip), place the uploads folder inside the zip into the backend's folder.  
Every user's password is test  
Few emails:  
* bob@bob.bob
* colorful@color.color
* wil@will.com


## Note
The backend reindexes the entire database on startup, which is not recommended for production use.
To disable in search/search.service.ts onModuleInit() change `await this.reindexDatabase();` to `await this.createIndex();`

## Api
Check out the swagger documentation on /docs for the api documentation.


## Database Model
![database model](https://github.com/TanyasiPills/ColorSync/blob/main/Example%20Database/database%20model.png)
