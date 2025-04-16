![Logo](docs/FilledTitleSided.png)

This is **ColorSync**, a collaborative art platform built as part of a school assignment.  
It allows artists to draw together in real time, save work locally, and sync with the platform's backend.

For full documentation and setup instructions, check out the [Wiki](https://github.com/TanyasiPills/ColorSync/wiki)

## Installation
### Desktop
Using the git repository source code and not installing via the installers or through the website you need the following steps:

#### Step 1: Downloading the repository
```
git clone https://github.com/TanyasiPills/ColorSync.git  
```
&nbsp;&nbsp;&nbsp;&nbsp; For only the desktop version, you can use:
```
git clone -b Desktop https://github.com/TanyasiPills/ColorSync.git  
```

#### Step 2: Opening the solution in Visual studio
&nbsp;&nbsp;&nbsp;&nbsp; All widely used Visual Studio version are supported

#### Step 3: Build & Run
&nbsp;&nbsp;&nbsp;&nbsp; From the solution, you can build and run the program itself, or the program with the installer, creating an msi file to install

### Setup
In the settings menu(development menu), the server ip should be set to the ip of the server computer  
![image](https://github.com/user-attachments/assets/9be443f4-939a-4b04-a8f3-cdc4ffbd9db5)

---

### Backend

#### Requirements  
A mysql server on port 3306 (default) like [xampp](https://www.apachefriends.org/download.html)  
[ElasticSearch](https://www.elastic.co/downloads/elasticsearch?tech=rtp&pg=en-elasticsearch-page&plcmt=hero&cta=eswt-24510-a) on port 9200 (default)  

### Setup
1. Download the node packages  
`npm install`  
2. Create the `.env` file based on `.env.example `  
3. Generate the prisma client  
`npx prisma generate`  
4. Generate a database based on prisma  
`npx prisma db push` 

#### ElasticSearch
&nbsp;&nbsp;&nbsp;&nbsp; Place [our config]((https://github.com/TanyasiPills/ColorSync/blob/main/docs/elasticsearch.yml)) (or create your own) into `\config\elastisearch.yml`

### Running
Start mysql  
Start ElasticSearch (run `bin\elasticsearch.bat` (or `bin/elasticsearch` on Linux))  
Start the server `npm run start`

### Example Database
You can download a example database from [main/Example Database/colorsync.sql](https://github.com/TanyasiPills/ColorSync/blob/main/Example%20Database/colorsync.sql).  
You can download the images for the example database from [main/Example Database/uplodas.zip](https://github.com/TanyasiPills/ColorSync/blob/main/Example%20Database/uploads.zip), place the uploads folder inside the zip into the backend's folder.  
Every user's password is test  
Few emails:  
* bob@bob.bob
* colorful@color.color
* wil@will.com


### Note
The backend reindexes the entire database on startup, which is not recommended for production use.
To disable in search/search.service.ts onModuleInit() change `await this.reindexDatabase();` to `await this.createIndex();`

### Api
Check out the swagger documentation on /docs for the api documentation.


### Database Model
![database model](https://github.com/TanyasiPills/ColorSync/blob/main/Example%20Database/database%20model.png)

## License

This project is licensed under the MIT License  
See the [LICENSE](./LICENSE) file for more info!
