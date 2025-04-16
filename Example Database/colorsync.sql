-- phpMyAdmin SQL Dump
-- version 5.0.4
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Apr 16, 2025 at 01:02 PM
-- Server version: 10.4.17-MariaDB
-- PHP Version: 8.0.0

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `colorsync`
--

-- --------------------------------------------------------

--
-- Table structure for table `comment`
--

CREATE TABLE `comment` (
  `id` int(11) NOT NULL,
  `userId` int(11) NOT NULL,
  `postId` int(11) NOT NULL,
  `text` varchar(500) COLLATE utf8mb4_unicode_ci NOT NULL,
  `date` datetime(3) NOT NULL DEFAULT current_timestamp(3)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `comment`
--

INSERT INTO `comment` (`id`, `userId`, `postId`, `text`, `date`) VALUES
(1, 1, 1, 'yo', '2025-02-17 12:17:00.435'),
(2, 1, 2, 'I can fix that burito', '2025-02-17 12:17:23.287'),
(3, 1, 2, 'Hire me for the low price of $999.99', '2025-02-17 12:17:35.160'),
(4, 1, 2, 'I can fix everything, even buritos', '2025-02-17 12:18:02.047'),
(5, 1, 3, 'You need to stop with the cat images', '2025-02-17 12:18:13.472'),
(6, 3, 6, 'The fence is tottaly fine, no need to fix it', '2025-02-17 12:20:06.926'),
(7, 3, 4, 'Spinny it', '2025-02-17 12:20:31.896'),
(8, 3, 3, 'blahaj', '2025-02-17 12:20:50.343'),
(9, 3, 2, 'Bob if you can fix everything, can you fix my marriage?', '2025-02-17 12:21:20.296'),
(10, 1, 2, 'There are some things that even I cannot fix...', '2025-02-17 12:22:02.493'),
(11, 4, 18, 'nem', '2025-03-06 09:41:03.993'),
(12, 4, 18, 'nem', '2025-03-06 09:41:06.201'),
(13, 4, 18, 'de', '2025-03-06 09:44:58.816'),
(14, 4, 18, 'HENTAI', '2025-03-06 09:47:07.970'),
(15, 4, 18, 'thats a funny way to say \"you gonna die\"', '2025-03-06 09:53:28.854'),
(19, 4, 16, 'monker', '2025-03-18 07:55:33.520'),
(20, 4, 34, 'fak u', '2025-03-25 08:33:27.760'),
(21, 4, 34, 'fak u', '2025-03-25 08:33:29.612'),
(22, 4, 23, 'nem', '2025-03-25 09:28:21.470'),
(23, 4, 37, 'nem', '2025-03-25 09:29:05.682'),
(24, 7, 37, 'SZIA', '2025-03-26 08:25:00.513'),
(25, 7, 32, 'sahbwhefwbjberknfe', '2025-03-26 08:25:11.540'),
(26, 7, 32, 'efnbejhbjerk', '2025-03-26 08:25:16.652'),
(27, 7, 32, 'efbjhebjrbgjerjk', '2025-03-26 08:25:21.312'),
(28, 7, 32, 'rfgrbjkrbkt', '2025-03-26 08:25:24.276'),
(29, 1, 23, 'stab me', '2025-03-26 11:07:49.978'),
(30, 1, 23, 'why is it dupeing?', '2025-03-26 11:08:10.081'),
(31, 1, 34, 'fak u', '2025-03-26 11:13:03.351'),
(32, 1, 26, 'huge hotdog', '2025-03-26 11:14:19.540'),
(33, 1, 26, 'gigantic hotdog', '2025-03-26 11:14:39.380'),
(34, 1, 36, 'bob', '2025-03-26 11:15:08.407'),
(35, 1, 36, 'bob2', '2025-03-26 11:15:20.792'),
(36, 1, 33, 'big pp', '2025-03-26 11:18:23.957'),
(37, 1, 41, 'huge fixer', '2025-03-26 13:48:34.224'),
(38, 1, 42, 'bob', '2025-03-26 14:11:31.608'),
(39, 1, 46, 'OMG a maid :3', '2025-03-26 18:21:47.844'),
(40, 1, 47, 'where is the maid????', '2025-03-26 21:36:38.764'),
(41, 1, 47, 'there is no picture', '2025-03-26 21:37:02.290'),
(42, 4, 47, 'git gud', '2025-03-27 16:02:33.848'),
(43, 1, 47, 'The has cum', '2025-03-27 16:05:46.615'),
(44, 1, 47, 'The maid has cum*', '2025-03-27 16:05:59.545'),
(48, 4, 50, 'Yeeeee, REEEPOOOST', '2025-03-30 08:28:04.682'),
(49, 4, 50, 'why no workie? :c', '2025-03-30 08:31:51.539'),
(50, 4, 50, 'try 200446', '2025-03-30 08:32:28.838'),
(51, 4, 50, 'nya', '2025-03-30 08:33:31.283'),
(52, 4, 50, '???', '2025-03-30 08:33:48.624'),
(53, 4, 47, 'nem', '2025-03-30 09:55:16.979'),
(55, 1, 53, '#savenya', '2025-03-31 11:49:08.776'),
(56, 1, 53, 'does this close?', '2025-03-31 11:49:29.285'),
(57, 4, 55, 'Looks like glorified poop', '2025-04-01 07:24:44.258'),
(58, 4, 55, '+ #nocat', '2025-04-01 07:24:58.896'),
(59, 4, 46, 'a hot dog dog', '2025-04-01 10:35:54.250'),
(60, 4, 56, 'bruh', '2025-04-03 10:56:00.298'),
(61, 8, 56, 'smh', '2025-04-06 12:11:05.675'),
(64, 15, 52, 'nagyon jó', '2025-04-11 16:37:15.571'),
(65, 18, 53, 'the time :0', '2025-04-11 17:12:30.447'),
(66, 18, 53, 'the time :0', '2025-04-11 17:12:33.456'),
(67, 18, 53, 'the time :0', '2025-04-11 17:12:54.714'),
(68, 18, 53, 'the time :0', '2025-04-11 17:12:57.429'),
(70, 16, 57, 'nem', '2025-04-11 17:21:21.382'),
(71, 16, 56, 'nem', '2025-04-11 17:21:28.763'),
(73, 19, 56, 'very cool', '2025-04-11 17:28:51.824'),
(74, 20, 59, 'Nice', '2025-04-11 19:04:55.874'),
(75, 21, 60, 'pretty', '2025-04-11 19:15:44.186'),
(79, 21, 18, 'what if no?', '2025-04-15 14:21:01.695'),
(80, 21, 6, 'My, elephants need sturdier stuff like that, can you manage?', '2025-04-15 14:21:44.447'),
(81, 21, 73, 'ok', '2025-04-15 18:22:15.999');

-- --------------------------------------------------------

--
-- Table structure for table `image`
--

CREATE TABLE `image` (
  `id` int(11) NOT NULL,
  `path` varchar(191) COLLATE utf8mb4_unicode_ci NOT NULL,
  `userId` int(11) NOT NULL,
  `visibility` enum('public','private') COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'private',
  `date` datetime(3) NOT NULL DEFAULT current_timestamp(3),
  `forPost` tinyint(1) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `image`
--

INSERT INTO `image` (`id`, `path`, `userId`, `visibility`, `date`, `forPost`) VALUES
(1, '1739793154954-624411410.jpg', 2, 'public', '2025-02-17 11:52:34.973', 0),
(2, '1739793695531-642729944.jpg', 2, 'public', '2025-02-17 12:01:35.534', 0),
(4, '1739793773122-7602646.png', 2, 'public', '2025-02-17 12:02:53.126', 0),
(5, '1739793882619-994624028.jpg', 1, 'public', '2025-02-17 12:04:42.622', 0),
(6, '1739793921746-953960166.jpg', 1, 'public', '2025-02-17 12:05:21.749', 0),
(7, '1739793942571-63340915.jpg', 1, 'public', '2025-02-17 12:05:42.575', 0),
(16, '1740835613673-831445252.jpg', 4, 'public', '2025-03-01 13:26:53.828', 1),
(17, '1740835717796-635650479.png', 4, 'public', '2025-03-01 13:28:37.995', 1),
(18, '1740918076631-299613145.png', 4, 'public', '2025-03-02 12:21:17.291', 1),
(19, '1740918395566-617862005.jpg', 4, 'public', '2025-03-02 12:26:36.725', 1),
(20, '1740932529768-756358234.jpg', 4, 'public', '2025-03-02 16:22:10.357', 1),
(21, '1742291769330-411558784.jpg', 4, 'public', '2025-03-18 09:56:09.381', 1),
(22, '1742291894636-817595264.jpg', 4, 'public', '2025-03-18 09:58:14.706', 1),
(23, '1742291955681-273233294.jpg', 4, 'public', '2025-03-18 09:59:15.871', 1),
(24, '1742293396094-118479693.jpg', 4, 'public', '2025-03-18 10:23:16.103', 1),
(25, '1742723558571-769392839.jpg', 4, 'public', '2025-03-23 09:52:38.711', 0),
(26, '1742834567894-577975731.jpg', 2, 'public', '2025-03-24 16:42:48.094', 1),
(31, '1743253817332-621881516.jpg', 4, 'private', '2025-03-29 13:10:18.255', 0),
(32, '1743254325084-625213191.jpg', 4, 'private', '2025-03-29 13:18:45.248', 0),
(33, '1743319073108-429512020.png', 4, 'public', '2025-03-30 07:17:53.388', 1),
(34, '1743328237931-650366835.jpeg', 4, 'public', '2025-03-30 09:50:37.937', 1),
(35, '1743421059803-619734547.jpeg', 1, 'public', '2025-03-31 11:37:39.841', 0),
(36, '1743421069039-923024798.jpg', 1, 'public', '2025-03-31 11:37:49.085', 0),
(37, '1743421246042-784116593.jpg', 1, 'public', '2025-03-31 11:40:46.182', 0),
(38, '1743421269490-714561888.jpg', 1, 'private', '2025-03-31 11:41:09.492', 0),
(39, '1743421294834-356281298.jpg', 1, 'public', '2025-03-31 11:41:34.912', 0),
(40, '1743424112280-470152838.jpg', 5, 'private', '2025-03-31 12:28:32.535', 0),
(41, '1743425581375-162526127.jpeg', 5, 'private', '2025-03-31 12:53:01.380', 0),
(43, '1743430697736-634608165.jpg', 5, 'public', '2025-03-31 14:18:17.741', 0),
(45, '1743866977611-648622168.jpg', 8, 'public', '2025-04-05 15:29:37.649', 1),
(46, '1743935555978-320960500.jpg', 8, 'private', '2025-04-06 10:32:36.055', 0),
(47, '1743935562346-691283701.jpg', 8, 'private', '2025-04-06 10:32:42.390', 0),
(48, '1743935567581-362333505.png', 8, 'private', '2025-04-06 10:32:47.619', 0),
(49, '1744013968687-190161150.jpg', 8, 'private', '2025-04-07 08:19:28.756', 0),
(52, '1744389724607-938887336.jpg', 16, 'private', '2025-04-11 16:42:04.771', 0),
(53, '1744391536488-252567090.jpg', 18, 'private', '2025-04-11 17:12:16.613', 0),
(54, '1744392509994-221614654.jpg', 19, 'private', '2025-04-11 17:28:30.116', 0),
(55, '1744392558121-618872329.jpg', 19, 'public', '2025-04-11 17:29:18.261', 1),
(56, '1744398323025-110740391.jpg', 20, 'public', '2025-04-11 19:05:23.082', 1),
(57, '1744398343391-151890474.jpg', 20, 'private', '2025-04-11 19:05:43.416', 0),
(58, '1744398968613-696053777.jpg', 21, 'public', '2025-04-11 19:16:08.782', 1),
(59, '1744398988648-298623106.jpg', 21, 'private', '2025-04-11 19:16:28.883', 0),
(60, '1744399091371-595404691.jpg', 21, 'private', '2025-04-11 19:18:11.534', 0),
(61, '1744399101486-546649848.jpg', 21, 'private', '2025-04-11 19:18:21.598', 0),
(62, '1744462410068-126656743.jpg', 21, 'public', '2025-04-12 12:53:30.117', 0),
(63, '1744623078717-699817546.jpg', 4, 'private', '2025-04-14 09:31:19.214', 0),
(64, '1744623088113-832218777.png', 4, 'private', '2025-04-14 09:31:28.196', 0),
(65, '1744623099141-89395933.jpg', 4, 'private', '2025-04-14 09:31:39.232', 0),
(68, '1744657341506-775968450.jpg', 15, 'public', '2025-04-14 19:02:21.535', 1),
(69, '1744710352320-115214357.jpg', 15, 'private', '2025-04-15 09:45:52.345', 0),
(111, '1744724996300-2917390.jpg', 21, 'public', '2025-04-15 13:49:56.359', 0),
(113, '1744727562970-183953538.jpg', 21, 'public', '2025-04-15 14:32:42.986', 0),
(114, '1744729741371-57187158.jpg', 2, 'public', '2025-04-15 15:09:01.459', 1),
(116, '1744732351549-681798924.jpeg', 2, 'private', '2025-04-15 15:52:31.602', 0);

-- --------------------------------------------------------

--
-- Table structure for table `post`
--

CREATE TABLE `post` (
  `id` int(11) NOT NULL,
  `userId` int(11) NOT NULL,
  `imageId` int(11) DEFAULT NULL,
  `text` varchar(500) COLLATE utf8mb4_unicode_ci NOT NULL,
  `date` datetime(3) NOT NULL DEFAULT current_timestamp(3),
  `imageForPost` tinyint(1) NOT NULL DEFAULT 0,
  `likes` int(11) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `post`
--

INSERT INTO `post` (`id`, `userId`, `imageId`, `text`, `date`, `imageForPost`, `likes`) VALUES
(1, 2, 1, 'Yo what\'s up', '2025-02-17 12:01:12.898', 0, 0),
(2, 2, 2, 'Would you eat this buritio?', '2025-02-17 12:01:46.878', 0, 1),
(3, 2, 4, 'Aware of shark attacks', '2025-02-17 12:03:02.924', 0, 1),
(4, 1, 5, 'Doing some work with my spinny thing', '2025-02-17 12:05:11.871', 0, 1),
(5, 1, 6, 'What the tractor doing?', '2025-02-17 12:05:36.752', 0, 2),
(6, 1, 7, 'I could fix that, they should have hired me to fix it', '2025-02-17 12:05:59.152', 0, 0),
(16, 4, 17, 'am i shitposting?', '2025-03-01 13:28:38.049', 1, 1),
(18, 4, 20, 'she is coming for you', '2025-03-02 16:22:10.567', 1, 3),
(23, 4, 23, 'i will stab you', '2025-03-18 09:59:15.964', 1, 0),
(26, 4, 24, 'my breakfast', '2025-03-18 10:23:16.196', 1, 2),
(27, 4, NULL, 'no image', '2025-03-18 10:23:52.332', 0, 0),
(28, 4, NULL, 'no bitches', '2025-03-18 10:24:55.680', 0, 0),
(29, 4, NULL, 'small pp', '2025-03-18 10:25:25.797', 0, 0),
(30, 4, NULL, 'scroll up', '2025-03-18 10:36:53.243', 0, 0),
(31, 4, NULL, 'no scroll', '2025-03-18 10:37:04.790', 0, 0),
(32, 4, NULL, 'scroll no frfr rn no cap', '2025-03-18 10:38:33.501', 0, 1),
(33, 4, NULL, 'no pp', '2025-03-18 10:44:11.634', 0, 0),
(34, 4, NULL, 'no scroll this time on god frfr', '2025-03-18 10:45:25.221', 0, 0),
(36, 1, NULL, 'wa?', '2025-03-18 10:50:26.565', 0, 0),
(37, 1, NULL, 'wa?', '2025-03-18 10:50:48.353', 0, 2),
(38, 2, 26, 'cica', '2025-03-24 16:42:48.205', 1, 3),
(39, 4, NULL, 'nothing', '2025-03-25 10:17:11.512', 0, 1),
(40, 1, NULL, 'how many tags is too many?', '2025-03-26 11:52:50.020', 0, 0),
(41, 1, NULL, 'i can fix even tags', '2025-03-26 13:47:55.271', 0, 1),
(42, 1, NULL, 'bob', '2025-03-26 14:11:24.957', 0, 1),
(43, 4, NULL, 'ayyoo', '2025-03-26 17:47:24.799', 0, 0),
(44, 4, NULL, 'Ayyoo', '2025-03-26 17:48:13.082', 0, 0),
(45, 4, NULL, 'ayyooo', '2025-03-26 17:56:49.777', 0, 0),
(46, 4, 24, 'OMG, MAID', '2025-03-26 18:20:21.125', 0, 1),
(47, 4, 25, 'MAID, MAID, MAID', '2025-03-26 18:54:24.459', 0, 2),
(50, 4, 33, 'Repost to keep the app runing', '2025-03-30 07:17:53.542', 1, 0),
(52, 1, 36, 'bob', '2025-03-31 11:38:02.998', 0, 1),
(53, 1, 37, 'nyooooo the nya stocks are dropping', '2025-03-31 11:48:47.051', 0, 1),
(54, 5, NULL, 'post', '2025-03-31 12:54:01.977', 0, 1),
(55, 5, NULL, 'new pfp', '2025-03-31 14:15:33.510', 0, 3),
(56, 5, NULL, 'I can logout EZ', '2025-04-03 10:51:09.764', 0, 2),
(57, 8, 45, 'SOCIAL MEDIA LOOKS SICK', '2025-04-05 15:29:37.836', 1, 1),
(59, 19, 55, 'this is a cool sketch', '2025-04-11 17:29:18.293', 1, 2),
(60, 20, 56, 'Picture I took', '2025-04-11 19:05:23.154', 1, 1),
(61, 21, 58, 'Elefanttt', '2025-04-11 19:16:08.812', 1, 1),
(62, 21, 62, 'BABY', '2025-04-12 12:55:59.215', 0, 0),
(63, 15, 68, 'goat', '2025-04-14 19:02:21.842', 1, 0),
(68, 21, 111, 'Cutie', '2025-04-15 13:56:10.174', 0, 0),
(69, 2, 4, 'text', '2025-04-15 14:18:04.076', 0, 2),
(73, 2, 114, 'text', '2025-04-15 15:09:01.525', 1, 0),
(77, 21, 113, 'Sunset', '2025-04-15 18:13:49.873', 0, 0),
(78, 21, NULL, 'test', '2025-04-15 18:22:51.702', 0, 0),
(79, 21, NULL, 'Priv Image', '2025-04-16 08:57:59.243', 0, 0),
(80, 21, 111, 'priv', '2025-04-16 09:01:52.336', 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `tag`
--

CREATE TABLE `tag` (
  `name` varchar(191) COLLATE utf8mb4_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `tag`
--

INSERT INTO `tag` (`name`) VALUES
('baby'),
('bad'),
('bob'),
('building'),
('cat'),
('cica'),
('cool'),
('cute'),
('dj'),
('ele'),
('example'),
('EZ'),
('FINALLY'),
('fixing'),
('frfr'),
('funny'),
('good_image'),
('good_looking'),
('good_tag'),
('GTFO'),
('helpWanted'),
('hhh'),
('horse'),
('IAmAlmostDone'),
('logout'),
('look'),
('love'),
('maid'),
('momma'),
('monika'),
('moretags'),
('mysketch'),
('new'),
('nice'),
('nocap'),
('nya'),
('of'),
('pl'),
('playing'),
('random_tag'),
('sad'),
('save'),
('stock'),
('tag'),
('tag1'),
('tag10'),
('tag2'),
('tag3'),
('tag4'),
('tag5'),
('tag6'),
('tag7'),
('tag8'),
('tag9'),
('tenshi'),
('w'),
('waifu'),
('wow');

-- --------------------------------------------------------

--
-- Table structure for table `token`
--

CREATE TABLE `token` (
  `token` varchar(191) COLLATE utf8mb4_unicode_ci NOT NULL,
  `userId` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `token`
--

INSERT INTO `token` (`token`, `userId`) VALUES
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6IkJvYiBUaGUgQnVpbGRlciIsInN1YiI6MSwiaWF0IjoxNzM5NzkxNjg3fQ.ooGRL9L5wzH6EmnGypFRja8nSxpZodItAP6Ug9Cu52s', 1),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Im55YSIsInN1YiI6MiwiaWF0IjoxNzM5NzkxNzIzfQ.iP6SpvZQoMRv4UNJILojch6IFjM8PvJ_MrLkSH3aylw', 2),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6IldpbGxpYW0iLCJzdWIiOjMsImlhdCI6MTczOTc5MTc5N30.jnmf9dSsXltax40HQwpOgMrzScImhkNF-kXzE4_iokE', 3),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6IlRlbnNoaS1zYW1hIiwic3ViIjo0LCJpYXQiOjE3NDA0OTMzNDN9.7c-0IdvJ33VWQ5Fyb_7jx4PhV_hrh-CwpRyQJRx7Nz8', 4),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Im55YW5kcm9pZCIsInN1YiI6NSwiaWF0IjoxNzQwODQ0MTI1fQ.iO_BZ1n8GsFbpKS4AwlOMYmLEKQjXAnhBbOW4ktYQMM', 5),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Im1vbmlrYSIsInN1YiI6NiwiaWF0IjoxNzQyMjg0NjA3fQ.-fxl46QR1eL2cRZccP67s84NPCBe6_t52BLWcEeHG7k', 6),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImN1cnNlZE1hdHlpIiwic3ViIjo3LCJpYXQiOjE3NDI5NzcyNDN9.RiEaA03U1uSfinMYiMNUaMMJz-PLStRqeJ2uMTRWKF8', 7),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6IlZhbGFraSIsInN1YiI6OCwiaWF0IjoxNzQzNzU0Mzc1fQ.BdD_Cvhc0Xu1AYuWweJHgOQ1QF1iN-Hn4CUazExXGQA', 8),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImFzZCIsInN1YiI6OSwiaWF0IjoxNzQ0MDQwNTUwfQ.MZVS5-R-Vd7gFRvgMD42J-eAClzwNn3xbr3W7zdrSiY', 9),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6InRlc3QiLCJzdWIiOjEwLCJpYXQiOjE3NDQyMjM5ODl9.3WbpBLjdYbKCnGNao5xVWKlsqOTeDmlNpkuVb9AdeC8', 10),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6InRlc3QyIiwic3ViIjoxMywiaWF0IjoxNzQ0MjI0MjA0fQ.JqwE8jzsPWoew2whPxnmgotWc3k0iOtgZKG7Vklu-5s', 13),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6InRlc3QzIiwic3ViIjoxNCwiaWF0IjoxNzQ0MjI0MjgzfQ.a_8WjTgtQM2geMhv5fh_3CcypFpQVf3IpH-mDIXZhPQ', 14),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6IkLDqWxhIiwic3ViIjoxNSwiaWF0IjoxNzQ0Mzg5MzYxfQ.cP7j7VlkmW2TiEbQvHpIsvi-ZGc18qHkpgp5DPvPEdU', 15),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6IkV4YW1wbGUgcHJvZmlsZSIsInN1YiI6MTYsImlhdCI6MTc0NDM4OTcwNH0.NGRxNtTe2IBkmtNfP9Rzzk1anh38ZeHGrkXLAMASC_Q', 16),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6IkV4YW1wbGVVc2VyIiwic3ViIjoxNywiaWF0IjoxNzQ0MzkxMDI4fQ.SnJQDOVoco-d45ZOjuKo4CA7Na-xteGxXPHJFqcaZxU', 17),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Im5ld1VzZXIiLCJzdWIiOjE4LCJpYXQiOjE3NDQzOTE1MDN9.vL4yN-wrvMIFExSrm401eVCF1frSgdj3bJubUQjHVD0', 18),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6InVzZXJUZXN0Iiwic3ViIjoxOSwiaWF0IjoxNzQ0MzkyNDc3fQ.Vmz1rZ7nv-XPIOuhMBBy47YaohQw3up2RUSC_s-Lqhk', 19),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6IkNvbG9yIiwic3ViIjoyMCwiaWF0IjoxNzQ0Mzk4MjgxfQ.lzu-AZzrQM7eR8vXbs5sJ3wITuUN9s9Jw2bfCpBPEVw', 20),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6IkVsZXBoYW50TG92ZXIiLCJzdWIiOjIxLCJpYXQiOjE3NDQzOTg5MjV9.Ri5J_vikP7MZfd-XLl6fNHevL5CbxZamT8lMeRPRz2w', 21),
('eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6InRlc3QiLCJzdWIiOjIyLCJpYXQiOjE3NDQ4MDExODB9.pMcnO-s1u20UYsSefI2OCjJbrYrK7Cd2opcSTOaRrG8', 22);

-- --------------------------------------------------------

--
-- Table structure for table `user`
--

CREATE TABLE `user` (
  `id` int(11) NOT NULL,
  `username` varchar(24) COLLATE utf8mb4_unicode_ci NOT NULL,
  `email` varchar(191) COLLATE utf8mb4_unicode_ci NOT NULL,
  `password` varchar(191) COLLATE utf8mb4_unicode_ci NOT NULL,
  `profile_picture` varchar(191) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `profile_description` varchar(191) COLLATE utf8mb4_unicode_ci DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `user`
--

INSERT INTO `user` (`id`, `username`, `email`, `password`, `profile_picture`, `profile_description`) VALUES
(1, 'Bobinho', 'bob@bob.bob', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1739793862403-919890144.jpg', 'I can fix everything\r\nCall me if you need any bobs\r\n(Even I can\'t fix your marriage)\r\n'),
(2, 'nyan', 'nya@nya.nya', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1739791856440-689912158.png', NULL),
(3, 'William', 'wil@will.com', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', NULL, NULL),
(4, 'Tenshi-sama', 'tenshi@nya.nya', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1744391524209-135178271.jpg', 'I love jazz and cats'),
(5, 'nyandroid', 'nya@and.nya', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1744223724832-204011500.jpeg', 'nya\nandroid best'),
(6, 'monika', 'monika@ddlc.com', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', NULL, NULL),
(7, 'cursedMatyi', 'cursed@cursed.cursed', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', NULL, NULL),
(8, 'Mdzs_lover', 'mdzs@lover.me', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1744017546166-915682395.jpg', 'yes I love this 2 man'),
(9, 'asdfg', 'asd@asd.asd', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', NULL, NULL),
(10, 'test', 'test@test.com', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', NULL, NULL),
(13, 'test2', 'test2@test.com', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', NULL, NULL),
(14, 'hhh', 'hhh@hhh.hhh', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1744224363179-829009112.jpg', 'kdhd\nshdh'),
(15, 'nem béla', 'bela@gmail.com', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1744389402829-148991156.jpg', 'nagyon béla'),
(16, 'Example profile', 'example@gmail.com', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1744391413866-196724494.jpg', 'nem\nde igen'),
(17, 'ExampleUser', 'exampleStuff@gmail.com', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1744391052323-108042604.jpg', NULL),
(18, 'newUser', 'newUser@gmail.com', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1744391524209-135178271.jpg', NULL),
(19, 'userTest', 'test@gmail.com', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1744392499772-971710777.jpg', NULL),
(20, 'Color', 'colorful@color.color', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1744398354123-826085796.jpg', 'HII'),
(21, 'LoVeSElephants', 'elephant@trunk.mail', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', '1744399024255-347816545.jpg', 'I love elephants'),
(22, 'test', 'test@test.test', '$argon2id$v=19$m=65536,t=3,p=4$thz0JYwQpnUjzOeleup9Cw$VZhxXJHZXphkx/2YmsQSyDDcmZnvXhLNvCa3ILFjWdk', NULL, NULL);

-- --------------------------------------------------------

--
-- Table structure for table `_likedposts`
--

CREATE TABLE `_likedposts` (
  `A` int(11) NOT NULL,
  `B` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `_likedposts`
--

INSERT INTO `_likedposts` (`A`, `B`) VALUES
(2, 4),
(3, 21),
(4, 5),
(5, 4),
(5, 5),
(16, 4),
(18, 1),
(18, 4),
(18, 21),
(26, 1),
(26, 4),
(32, 7),
(37, 7),
(37, 21),
(38, 5),
(38, 7),
(38, 21),
(39, 7),
(41, 1),
(42, 5),
(46, 1),
(47, 1),
(47, 4),
(52, 15),
(53, 1),
(54, 8),
(55, 5),
(55, 8),
(55, 19),
(56, 5),
(56, 8),
(57, 8),
(59, 4),
(59, 20),
(60, 21),
(61, 21),
(69, 2),
(69, 21);

-- --------------------------------------------------------

--
-- Table structure for table `_posttotag`
--

CREATE TABLE `_posttotag` (
  `A` int(11) NOT NULL,
  `B` varchar(191) COLLATE utf8mb4_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `_posttotag`
--

INSERT INTO `_posttotag` (`A`, `B`) VALUES
(1, 'cat'),
(2, 'cat'),
(3, 'cat'),
(3, 'funny'),
(18, 'GTFO'),
(18, 'monika'),
(23, 'tag1'),
(23, 'tag2'),
(23, 'tag3'),
(26, 'tag1'),
(26, 'tag2'),
(26, 'tag3'),
(27, 'tag1'),
(27, 'tag2'),
(27, 'tag3'),
(28, 'tag1'),
(28, 'tag2'),
(28, 'tag3'),
(29, 'tag1'),
(29, 'tag2'),
(29, 'tag3'),
(30, 'tag1'),
(30, 'tag2'),
(30, 'tag3'),
(31, 'tag1'),
(31, 'tag2'),
(31, 'tag3'),
(32, 'tag1'),
(32, 'tag2'),
(32, 'tag3'),
(33, 'tag1'),
(33, 'tag2'),
(33, 'tag3'),
(34, 'tag1'),
(34, 'tag2'),
(34, 'tag3'),
(36, 'tag1'),
(36, 'tag2'),
(36, 'tag3'),
(37, 'tag1'),
(37, 'tag2'),
(37, 'tag3'),
(38, 'cica'),
(38, 'dj'),
(39, 'example'),
(39, 'pl'),
(40, 'tag1'),
(40, 'tag10'),
(40, 'tag2'),
(40, 'tag3'),
(40, 'tag4'),
(40, 'tag5'),
(40, 'tag6'),
(40, 'tag7'),
(40, 'tag8'),
(40, 'tag9'),
(41, 'bob'),
(41, 'building'),
(41, 'fixing'),
(42, 'bob'),
(46, 'good_looking'),
(46, 'monika'),
(46, 'waifu'),
(47, 'good_looking'),
(47, 'maid'),
(47, 'monika'),
(47, 'moretags'),
(50, 'good_tag'),
(50, 'helpWanted'),
(50, 'sad'),
(53, 'nya'),
(53, 'save'),
(53, 'stock'),
(56, 'EZ'),
(56, 'frfr'),
(56, 'logout'),
(56, 'nocap'),
(56, 'nya'),
(56, 'w'),
(57, 'FINALLY'),
(57, 'IAmAlmostDone'),
(59, 'cool'),
(59, 'mysketch'),
(60, 'look'),
(60, 'new'),
(60, 'nice'),
(60, 'wow'),
(62, 'baby'),
(62, 'ele'),
(68, 'baby'),
(68, 'momma'),
(68, 'playing'),
(69, 'tag1'),
(69, 'tag2'),
(73, 'tag1'),
(73, 'tag2'),
(78, 'bad');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `comment`
--
ALTER TABLE `comment`
  ADD PRIMARY KEY (`id`),
  ADD KEY `Comment_userId_fkey` (`userId`),
  ADD KEY `Comment_postId_fkey` (`postId`);

--
-- Indexes for table `image`
--
ALTER TABLE `image`
  ADD PRIMARY KEY (`id`),
  ADD KEY `Image_userId_fkey` (`userId`);

--
-- Indexes for table `post`
--
ALTER TABLE `post`
  ADD PRIMARY KEY (`id`),
  ADD KEY `Post_userId_fkey` (`userId`),
  ADD KEY `Post_imageId_fkey` (`imageId`);

--
-- Indexes for table `tag`
--
ALTER TABLE `tag`
  ADD PRIMARY KEY (`name`),
  ADD UNIQUE KEY `Tag_name_key` (`name`);

--
-- Indexes for table `token`
--
ALTER TABLE `token`
  ADD UNIQUE KEY `Token_token_key` (`token`),
  ADD KEY `Token_userId_fkey` (`userId`);

--
-- Indexes for table `user`
--
ALTER TABLE `user`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `User_email_key` (`email`);

--
-- Indexes for table `_likedposts`
--
ALTER TABLE `_likedposts`
  ADD UNIQUE KEY `_LikedPosts_AB_unique` (`A`,`B`),
  ADD KEY `_LikedPosts_B_index` (`B`);

--
-- Indexes for table `_posttotag`
--
ALTER TABLE `_posttotag`
  ADD UNIQUE KEY `_PostToTag_AB_unique` (`A`,`B`),
  ADD KEY `_PostToTag_B_index` (`B`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `comment`
--
ALTER TABLE `comment`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=82;

--
-- AUTO_INCREMENT for table `image`
--
ALTER TABLE `image`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=117;

--
-- AUTO_INCREMENT for table `post`
--
ALTER TABLE `post`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=81;

--
-- AUTO_INCREMENT for table `user`
--
ALTER TABLE `user`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=23;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `comment`
--
ALTER TABLE `comment`
  ADD CONSTRAINT `Comment_postId_fkey` FOREIGN KEY (`postId`) REFERENCES `post` (`id`) ON DELETE NO ACTION ON UPDATE CASCADE,
  ADD CONSTRAINT `Comment_userId_fkey` FOREIGN KEY (`userId`) REFERENCES `user` (`id`) ON DELETE NO ACTION ON UPDATE CASCADE;

--
-- Constraints for table `image`
--
ALTER TABLE `image`
  ADD CONSTRAINT `Image_userId_fkey` FOREIGN KEY (`userId`) REFERENCES `user` (`id`) ON DELETE NO ACTION ON UPDATE CASCADE;

--
-- Constraints for table `post`
--
ALTER TABLE `post`
  ADD CONSTRAINT `Post_imageId_fkey` FOREIGN KEY (`imageId`) REFERENCES `image` (`id`) ON DELETE NO ACTION ON UPDATE CASCADE,
  ADD CONSTRAINT `Post_userId_fkey` FOREIGN KEY (`userId`) REFERENCES `user` (`id`) ON DELETE NO ACTION ON UPDATE CASCADE;

--
-- Constraints for table `token`
--
ALTER TABLE `token`
  ADD CONSTRAINT `Token_userId_fkey` FOREIGN KEY (`userId`) REFERENCES `user` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `_likedposts`
--
ALTER TABLE `_likedposts`
  ADD CONSTRAINT `_LikedPosts_A_fkey` FOREIGN KEY (`A`) REFERENCES `post` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `_LikedPosts_B_fkey` FOREIGN KEY (`B`) REFERENCES `user` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `_posttotag`
--
ALTER TABLE `_posttotag`
  ADD CONSTRAINT `_PostToTag_A_fkey` FOREIGN KEY (`A`) REFERENCES `post` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `_PostToTag_B_fkey` FOREIGN KEY (`B`) REFERENCES `tag` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
