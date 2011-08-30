#include <QtTest/QtTest>
#include <pgnstream.h>
#include <pgngame.h>


class tst_PgnGame: public QObject
{
	Q_OBJECT
	
	private slots:
		void parser_data() const;
		void parser();
};

void tst_PgnGame::parser_data() const
{
	QTest::addColumn<QByteArray>("pgn");
	QByteArray pgn;

	pgn = "[Event \"?\"]\n"
	      "[Site \"Linares\"]\n"
	      "[Date \"1993.??.??\"]\n"
	      "[Round \"0.12\"]\n"
	      "[White \"Karpov,An\"]\n"
	      "[Black \"Kramnik,V\"]\n"
	      "[Result \"1/2-1/2\"]\n"
	      "[ECO \"B13\"]\n\n"
	      "1. c4 c6 2. e4 d5 3. exd5 cxd5 4. d4 Nf6 5. Nc3 Nc6 6. Nf3 Bg4 7. cxd5\n"
	      "Nxd5 8. Qb3 Bxf3 9. gxf3 e6 10. Qxb7 Nxd4 11. Bb5+ Nxb5 12. Qc6+ Ke7\n"
	      "13. Qxb5 Qd7 14. Nxd5+ Qxd5 15. Bg5+ f6 16. Qxd5 exd5 17. Be3 Ke6\n"
	      "18. O-O-O Bb4 19. Rd3 Rhd8 20. a3 Rac8+ 21. Kb1 Bc5 22. Re1 Kd6 23. Rg1\n"
	      "g6 24. Rgd1 Ke6 25. Re1 Bxe3 26. Rdxe3+ Kf5 27. Re7 Kf4 28. R1e3 a5\n"
	      "29. h3 h5 30. R7e6 Kg5 31. Ra6 d4 32. f4+ Kf5 33. Rxa5+ Kxf4 34. Rd3 Ke4\n"
	      "35. Rd2 g5 36. Ra6 f5 37. Re6+ Kf3 38. Re5 Kf4 39. Re6 h4 40. Rd3 g4\n"
	      "41. Rh6 Kg5 42. Rh7 Rc6 43. a4 Rd5 44. a5 Rcd6 45. Ra7 gxh3 46. Rg7+ Kf4\n"
	      "47. Rh7 Ke4 48. Rxh3 Rxa5 49. Kc2 Rb5 50. Re7+ Kf4 51. Rxh4+ Kf3\n"
	      "52. Rh3+ Kxf2 53. Rd3 Rc6+ 54. Kb1 Rb4 55. b3 f4 56. Re4 Rf6 57. Kb2 f3\n"
	      "58. Ka3 Rbb6 59. Rdxd4 Rg6 60. Rd2+ Kg3 61. Re3 Rbe6 62. Rc3 Ra6+\n"
	      "63. Kb2 Rg4 64. Rd8 Rf6 65. Rd2 Rgf4 66. Ka3 Kg4 67. Rf2 Ra6+ 68. Kb2\n"
	      "Rh6 69. Ka3 Rh1 70. Rd3 Kg3 71. Rc2 Rhh4 72. Re3 Rh2 73. Rc8 Kg2\n"
	      "74. Rg8+ Kf1 75. b4 f2 76. Rb3 Rhh4 77. Rgg3 Rd4 78. Ka4 Rhe4 79. Ka5\n"
	      "Rd2 80. Rh3 Ke2 81. Rh2 Ra2+ 82. Kb6 Re6+ 83. Kc5 Rc2+ 84. Kb5 Rh6\n"
	      "85. Rg2 Rf6 86. Rh2 Rh6 87. Rg2 Kf1 88. Rg5 Rf6 89. Rc5 Rd2 90. Rc6 Rf4\n"
	      "91. Rc1+ Kg2 92. Rbb1 Rf8 93. Ka5 Ra2+ 94. Kb6 Rf6+ 95. Kc5 Rf5+ 96. Kb6\n"
	      "Re2 97. b5 Re6+ 98. Ka5 Rfe5 99. Ka4 Re4+ 1/2-1/2\n";
	QTest::newRow("game1") << pgn;

	pgn = "[Event \"CCRL 40/40\"]\n"
	      "[Site \"CCRL\"]\n"
	      "[Date \"2009.03.01\"]\n"
	      "[Round \"164.1.156\"]\n"
	      "[White \"Cheese 1.3\"]\n"
	      "[Black \"Chezzz 1.0.3\"]\n"
	      "[Result \"0-1\"]\n"
	      "[ECO \"C15\"]\n"
	      "[Opening \"French\"]\n"
	      "[Variation \"Winawer (Nimzovich) variation\"]\n"
	      "[PlyCount \"102\"]\n"
	      "[WhiteElo \"2408\"]\n"
	      "1. e4 e6 2. d4 d5 3. Nc3 Bb4 4. Qd3 Ne7 5. Ne2 c5 6. Bg5 f6 7. Bd2 Nbc6 8.\n"
	      "O-O-O {-0.23/13 33s} O-O {+0.03/12 45s} 9. a3 {-0.13/13 28s} c4 {+0.11/13 45s}\n"
	      "10. Qg3 {+0.04/13 46s} Ba5 {+0.20/12 45s} 11. f3 {+0.04/13 45s} a6 {+0.05/12\n"
	      "45s} 12. h4 {+0.17/13 37s} b5 {+0.14/13 45s} 13. h5 {+0.26/13 46s} Bxc3\n"
	      "{+0.31/13 45s} 14. Bxc3 {+0.13/14 44s} a5 {+0.33/13 45s} 15. h6 {+0.24/13 46s}\n"
	      "g6 {+0.42/13 45s} 16. Bd2 {+0.12/14 33s} b4 {+0.50/13 45s} 17. a4 {+0.12/12\n"
	      "29s} b3 {+0.82/13 45s} 18. c3 {-0.36/14 47s} Qd7 {+1.02/15 113s} 19. Bf4\n"
	      "{-0.40/14 48s} Na7 {+1.01/14 42s} 20. Bd6 {-0.70/15 48s} Rf7 {+1.15/14 42s} 21.\n"
	      "Kd2 {-1.08/14 26s} Bb7 {+1.50/14 84s} 22. Bxe7 {-0.50/14 49s} Rxe7 {+1.50/12\n"
	      "20s} 23. Nf4 {-0.73/14 49s} Kh8 {+1.53/13 41s} 24. exd5 {-0.98/13 49s} exd5\n"
	      "{+1.57/13 20s} 25. Nxg6+ {-0.95/14 49s} hxg6 {+2.31/14 21s} 26. Qxg6 {-1.15/15\n"
	      "31s} Qd6 {+2.44/14 43s} 27. Kc1 {-1.92/15 50s} Rg8 {+2.97/14 43s} 28. Qh5\n"
	      "{-2.36/15 50s} Qf4+ {+3.89/15 43s} 29. Kb1 {-3.69/17 36s} Rg5 {+4.02/15 263s}\n"
	      "30. Qh3 {-3.85/16 51s} Qd2 {+3.60/15 26s} 31. Bd3 {-4.28/16 30s} Qxg2 {+3.71/14\n"
	      "23s} 32. Qxg2 {-3.02/16 54s} Rxg2 {+3.81/15 11s} 33. Bf1 {-3.05/16 54s} Rc2\n"
	      "{+3.77/15 25s} 34. Ka1 {-3.49/16 54s} Bc6 {+3.62/14 25s} 35. Bh3 {-3.74/16 30s}\n"
	      "Bxa4 {+3.84/14 25s} 36. Rb1 {-3.80/15 58s} Nb5 {+3.87/14 25s} 37. Bf5 {-4.29/15\n"
	      "38s} Nxc3 {+3.93/15 25s} 38. Bxc2 {-5.02/17 65s} bxc2 {+4.19/14 12s} 39. bxc3\n"
	      "{-5.24/17 65s} cxb1=R+ {+4.76/14 42s} 40. Kxb1 {-5.35/17 32s} Bb3 {+4.76/13\n"
	      "19s} 41. Rh2 {-5.42/18 38s} Re3 {+5.16/14 37s} 42. Kb2 {-6.35/17 21s} Rxf3\n"
	      "{+5.37/14 37s} 43. Rg2 {-6.96/17 38s} Kh7 {+5.51/14 37s} 44. Rh2 {-7.15/20 38s}\n"
	      "f5 {+6.09/14 46s} 45. Re2 {-7.95/16 38s} Kxh6 {+5.96/13 37s} 46. Re6+ {-8.34/16\n"
	      "38s} Kg5 {+6.32/14 37s} 47. Re2 {-8.66/16 38s} Kf4 {+6.66/13 37s} 48. Rg2\n"
	      "{-10.04/17 38s} Ke3 {+8.12/13 37s} 49. Ka3 {-11.44/18 38s} f4 {+9.40/14 37s}\n"
	      "50. Rg5 {-12.37/18 38s} Rf1 {+9.91/13 37s} 51. Re5+ {-16.96/17 38s} Kd3\n"
	      "{+12.91/14 37s 0-1 Adjudication} 0-1\n";
	QTest::newRow("game2") << pgn;
}

void tst_PgnGame::parser()
{
	QFETCH(QByteArray, pgn);

	PgnStream stream(&pgn);
	PgnGame game;
	QBENCHMARK
	{
		QVERIFY(game.read(stream));
		stream.rewind();
	}
}

QTEST_MAIN(tst_PgnGame)
#include "tst_pgngame.moc"
