import { PrismaClient } from '@prisma/client';
import { readdirSync, unlinkSync } from 'fs';

const db = new PrismaClient();

async function main() {
    const paths: string[] = [];
    const images = await db.image.findMany({select: {path: true}});
    images.forEach(image => {
        paths.push(image.path);
    });
    const users = await db.user.findMany({select: {profile_picture: true}});
    users.forEach(user => {
        if (user.profile_picture) paths.push(user.profile_picture);
    });

    const files = await readdirSync('uploads');
    files.filter(file => file !== "cache").forEach(file => {
        if (!paths.includes(file)) {
            console.log(`Deleting ${file}`);
            try {
                unlinkSync(`uploads/${file}`);
            } catch (e) {
                console.error(e);
            }
        }
    });
}

main()
    .then(() => db.$disconnect())
    .catch(e => {
        console.error(e);
        db.$disconnect();
    });