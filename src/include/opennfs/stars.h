#ifndef _STARS_H_
#define _STARS_H_

namespace astronomy {

static float star_map[] = {

 /* Ursa Minor, Ursae Minoris, UMi */
         2.07, 89.09, 2.02,    /* 1 ,    0       */
        17.40, 86.36, 4.36,    /* 23,    1       */  
        16.48, 82.05, 4.23,    /* 22,    2       */
        15.44, 77.52, 4.32,    /* 16,    3       */
        14.50, 74.15, 2.08,    /* 7      4       */   
        15.20, 71.55, 3.05,    /* 13     5       */
 /* Virgo, Virginis , Vir */
        13.00, 11.06, 2.84,    /* 47     6       */
        12.54,  3.32, 3.38,    /* 43     7       */
        13.33, -0.28, 3.38,    /* 79     8       */ 
        12.00,  8.40, 4.04,    /* ??     9       */
        12.40, -1.19, 2.74,    /* 29    10       */ 
        12.18, -0.32, 3.90,    /* 15    11       */
        11.49,  1.54, 3.60,    /* 5     12       */  
        13.08, -5.24, 4.38,    /* 51    13       */ 
        13.23,-11.02, 0.97,    /* 67    14       */
 /* Ursa Maior, Ursae Maioris, UMa */
        13.46, 49.26, 1.86,    /* 85    15       */
        13.22, 55.03, 2.06,    /* 79AB  16       */ 
        12.52, 56.06, 1.77,    /* 77    17       */
        12.14, 57.10, 3.31,    /* 69    18       */
        11.02, 61.53, 1.79,    /* 50    19       */
        11.00, 56.31, 2.37,    /* 48    20       */
        11.52, 53.50, 2.44,    /* 64    21       */ 
 /* Orion, Orionis, Ori */ 
         5.46, -9.41, 2.05,    /* 53    22       */
         5.39, -1.57, 1.77,    /* 50    23       */ 
         5.53,  7.24, 0.42,    /* 58    24       */
         5.34,  9.55, 3.39,    /* 39    25       */
         5.23,  6.20, 1.46,    /* 24    26       */ 
         5.30, -0.19, 2.24,    /* 34    27       */  
         5.13, -8.14, 0.13,    /* 19    28       */
         5.34, -1.13, 1.69,    /* 46    29       */
 /* Taurus, Tauri, Tau */
         4.34, 16.28, 0.86,    /* 87    30       */
         4.27, 19.08, 3.54,    /* 74    31       */
         4.18, 15.34, 3.65,    /* 54    32       */
         5.24, 28.35, 1.65,    /* 112   33       */
         5.36, 21.08, 3.03,    /* 123   34       */
         3.25,  9.35, 3.75,    /* 2     35       */
         3.47, 24.04, 5.0,     /* BU    36       */
 /* Lyra, Lyrae, Lyr */
        18.36, 38.46, 0.03,    /* 3     37       */
        18.43, 37.35, 4.36,    /* 6     38       */
        18.49, 33.20, 3.42,    /* 10    39       */
        18.58, 32.39, 3.24,    /* 14    40       */
        18.54, 36.52, 6.2,     /* d2    41       */  
 /* Cygnus, Cygni, Cyg */
        20.40, 45.11, 1.25,    /* 50    42       */
        20.21, 40.11, 2.23,    /* 37    43       */ 
        19.44, 45.04, 2.87,    /* 18    44       */
        19.16, 53.19, 3.76,    /* 1     45       */
        20.45, 33.53, 2.46,    /* 53    46       */
        21.11, 30.07, 3.20,    /* 64    47       */
        19.29, 27.54, 3.08,    /* 6     48       */  
 /* Aquila, Aquilae, Aql */
        19.49,  8.48, 0.76,    /* 53    49       */
        19.24,  3.04, 3.36,    /* 30    50       */    
        19.04, -4.55, 3.43,    /* 16    51       */
        19.04, 13.50, 2.99,    /* 17    52       */
        20.10,  0.54, 3.24,    /* 65    53       */
 /* Cassiopeia, Cassiopeiae, Cas */
         1.52, 63.33, 3.38,    /* 45    54       */
         1.24, 60.06, 2.68,    /* 37    55       */         
         0.55, 60.35, 2.39,    /* 27    56       */
         0.39, 56.24, 2.23,    /* 18    57       */
         0.07, 59.01, 2.37,    /* 11    58       */
 /* Andromeda, Andromedae, And */
         2.02, 42.13, 2.10,    /* 57    59       */
         1.08, 35.29, 2.05,    /* 43    60       */
         0.07, 28.57, 2.06,    /* 21    61       */
         0.55, 38.22, 3.87,    /* 37    62       */  
         0.48, 40.57, 4.40,    /* 35    63       */
 /* Hercules, Herculis, Her */
        17.14, 36.50, 3.16,    /* 67    64       */
        16.42, 38.58, 3.50,    /* 44    65       */
        16.40, 31.39, 2.81,    /* 40    66       */
        16.59, 30.58, 3.92,    /* 58    67       */  
        17.55, 37.15, 3.87,    /* 91    68       */  
        17.38, 46.01, 3.80,    /* 85    69       */  
        16.19, 46.22, 3.90,    /* 22    70       */
        16.08, 45.00, 4.27,    /* 11    71       */
        16.29, 21.33, 2.74,    /* 27    72       */
        17.13, 14.25, 3.06,    /* 64    73       */
        16.20, 19.13, 3.76,    /* 20    74       */ 
        17.14, 24.52, 3.13,    /* 65    75       */
        17.29, 26.08, 4.41,    /* 76    76       */
        17.45, 27.44, 3.42,    /* 86    77       */
        17.57, 30.12, 4.41,    /* 94    78       */ 
 /* Leo, Leonis, Leo */
         9.44, 23.53, 2.98,    /* 17    79       */
         9.51, 26.08, 3.88,    /* 24    80       */
        10.15, 23.33, 3.44,    /* 36    81       */
        10.18, 19.58, 1.98,    /* 41    82       */
        10.07, 12.05, 1.35,    /* 32    83       */
        10.06, 16.53, 3.53,    /* 30    84       */
        11.12, 20.40, 2.56,    /* 68    85       */  
        11.47, 14.43, 2.14,    /* 94    86       */ 
        11.12, 15.34, 3.35,    /* 70    87       */
 /* Cepheus, Cephei, Cep */
        23.38, 77.30, 3.21,    /* 35    88       */
        22.48, 66.04, 3.53,    /* 32    89       */ 
        22.28, 58.17, 4.34,    /* 27    90       */
        21.18, 62.29, 2.45,    /* 5     91       */
        21.28, 70.27, 3.23,    /* 8     92       */
        20.44, 61.44, 3.43,    /* 3     93       */ 
        20.29, 62.55, 4.28,    /* 2     94       */
        22.10, 58.05, 3.35,    /* 21    95       */
 /* Pegasus, Pegasi, Peg */
        21.30,  9.46, 2.39,    /* 8     96       */
        22.08,  6.04, 3.52,    /* 26    97       */
        22.40, 10.42, 3.40,    /* 42    98       */
        23.03, 15.04, 2.48,    /* 54    99       */   
         0.11, 15.03, 2.84,    /* 88   100       */
        23.02, 27.56, 2.50,    /*     101       */
        22.41, 30.05, 2.95,    /* 44   102       */
        22.48, 24.28, 3.48,    /* 48   103       */
 /* Bootes, Bootis, Boo */
        14.14, 19.19,-0.05,    /* 16   104       */
        14.30, 30.29, 3.59,    /* 25   105       */ 
        14.31, 38.25, 3.02,    /* 27   106       */
        15.01, 40.29, 3.50,    /* 42   107       */
        15.14, 33.24, 3.49,    /* 49   108       */
        14.44, 27.11, 2.37,    /* 36   109       */
        14.40, 13.49, 3.78,    /* 30   110       */
 /* Corona Borealis, Coronae Borealis, CrB */
        15.56, 26.57, 4.15,    /* 13   111       */
        15.42, 26.22, 3.85,    /* 8    112       */ 
        15.33, 26.48, 2.24,    /* 5    113       */
        15.26, 29.11, 3.68,    /* 3    114       */
        15.31, 31.27, 4.13,    /* 4    115       */
 /* Crater, Crateri, Crt */
        11.35, -9.8,  5.00,    /* ??   116       */
        11.25,-10.05, 4.50,    /* ??   117       */
        11.20,-14.39, 3.56,    /* 12   118       */
        11.23,-17.33, 4.08,    /* 15   119       */
        11.38,-18.00, 4.50,    /* ??   120       */
        11.55,-17.30, 4.50,    /* ??   121       */
        10.58,-18.10, 4.07,    /* 7    122       */
        11.10,-22.41, 4.48,    /* 11   123       */
 /* Libra, Librae, Lib */
        15.02,-25.11, 3.27,    /* 20   124       */
        14.49,-15.56, 2.75,    /* 9    125       */ 
        15.15, -9.18, 2.61,    /* 27   126       */
        15.34,-14.42, 3.91,    /* 38   127       */
 /* Pisces, Piscium, Psc */
         1.10, 29.57, 4.51,    /* 83   128       */
         1.30, 15.13, 3.62,    /* 99   129       */
         1.44,  9.02, 4.26,    /* 110  130       */
         2.01,  2.38, 3.82,    /* 113  131       */
         1.40,  5.22, 4.44,    /* 106  132       */ 
         1.01,  7.45, 4.28,    /* 71   133       */
         0.47,  7.27, 4.44,    /* 63   134       */
        23.58,  6.44, 4.01,    /* 28   135       */
        23.38,  5.29, 4.13,    /* 17   136       */
        23.26,  6.14, 4.28,    /* 10   137       */
        23.15,  3.09, 3.69,    /* 6    138       */
        23.26,  0.57, 5.90,    /* ??   139       */   
        23.02,  3.41, 4.52,    /* 4    140       */
 /* Aquarius, Aquarii, Aqr */
        23.13, -6.11, 4.22,    /* 90   141       */
        23.14, -9.13, 4.25,    /* 91   142       */       
        23.41,-14.41, 4.51,    /* 105  143       */
        23.21,-20.14, 3.98,    /* 98   144       */
        23.08,-21.19, 3.64,    /* 88   145       */
        22.53,-15.57, 3.28,    /* 76   146       */
        22.48,-13.44, 3.98,    /* 71   147       */
        22.51, -7.43, 3.79,    /* 73   148       */
        22.15, -7.55, 4.15,    /* 43   149       */ 
        22.05,-14.00, 4.25,    /* 33   150       */
        22.20, -1.31, 3.84,    /* 48   151       */
        22.04, -0.27, 2.93,    /* 34   152       */
        21.30, -5.41, 2.87,    /* 22   153       */
        21.08,-11.28, 4.52,    /* 13   154       */
        20.46, -9.35, 3.77,    /* 2    155       */
 /* Gemini, Geminorum, Gem */
         6.36, 16.25, 1.92,    /* 24   156       */
         6.21, 22.32, 2.87,    /* 13   157       */        
         6.42, 25.09, 2.98,    /* 27   158       */
         7.09, 30.18, 4.40,    /* 46   159       */
         7.33, 31.57, 1.58,    /* 66   160       */
         7.43, 28.05, 1.14,    /* 78   161       */
         7.42, 24.28, 3.57,    /* 77   162       */
         7.18, 22.02, 3.53,    /* 55   163       */
 /* Scorpius, Scorpii, Sco */
        16.50,-38.00, 3.03,    /* u1   164       */
        16.48,-34.15, 2.29,    /* 26   165       */
        16.34,-28.10, 2.81,    /* 23   166       */
        16.27,-26.23, 0.91,    /* 12   167       */
        16.19,-25.32, 2.88,    /* 20   168       */
        16.06,-20.36, 3.97,    /* 9    169       */
        15.58,-22.30, 2.32,    /* 7    170       */
        15.57,-26.03, 2.91    /* 6    171       */
 }; 

}

#endif /* _STARS_H_ */
