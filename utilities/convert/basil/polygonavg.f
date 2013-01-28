
      SUBROUTINE STRAINAVG(IM,EX,EY,EXREF,EYREF,WORK,
     :                     LEM,NOR,IPOLYNUM,
     :                     FAVG,AL,NE,NUP,IVERBOSE)
C
C     STRAINAVG calculates an area averaged deformation for
C     the polygonal region, specified by IM, using the
C     deformation for each mesh element.
C     Only valid for small strain increments.
C     (TRI is twice element area, ASUM is twice the polygon area)
C
C
C     Y1 CONTAINS COORDINATES OF UNDEFORMED ELEMENT
C
C     Y2 CONTAINS COORDINATES OF DEFORMED   ELEMENT
C
C     ORDER OF COORDINATES: X(PT1),Y(PT1), ....... ,X(PT3),Y(PT3)
C
C     NNN ALLOWS CHOICE OF RELATIVE POSITION VECTORS TO
C
C     BE PLACED IN X1 AND X2
C
C     NNN = 1:      (PT2 - PT1, PT3 - PT2) (PT2 is reference pt)
C
C     NNN = 2:      (PT2 - PT1, PT3 - PT1) (PT1 is reference pt)
C
C     NNN = 3:      (PT3 - PT2, PT3 - PT1) (PT3 is reference pt)
C
C
      DIMENSION EX(NUP),EY(NUP),EXREF(NUP),EYREF(NUP)
      DIMENSION LEM(6,NE),NOR(NUP)
      DIMENSION FAVG(4),IPOLYNUM(NE)
      DIMENSION WORK(NUP)
C     COMMON/CONTRO/LUW,LRBAT,LRINT,LDIN,LWINT,LPLOT,IAUTO,ILABEL
C    1 ,ICOLO
      DIMENSION Y1(6),Y2(6),X1(4),X2(4),F(4),N1(4),N2(4)
      DOUBLE PRECISION TRI,DXP(3),DYP(3),ASUM
      DOUBLE PRECISION DFAVG(4), DFAVGSUM(4)
      DATA PI /3.14159/
C
      ASUM = 0.0
      AL1SUM = 0.0
      AL2SUM = 0.0
      DO 10 N=1,4
  10  DFAVGSUM(N) = 0.0

C
C    For each triangle in the polygon
C

      DO 60 N=1,NE
      IF (IPOLYNUM(N).NE.IM) GO TO 60
C
C    Put the element coordinates into the Y1 and Y2 matrices
C

      DO 20 K=1,3
      LK=NOR(LEM(K,N))
      JJ=2*(K-1)+1
      Y1(JJ)=EXREF(LK)
      Y1(JJ+1)=EYREF(LK)
      Y2(JJ)=EX(LK)
      Y2(JJ+1)=EY(LK)
      DXP(K)=EX(LK)
      DYP(K)=EY(LK)
   20 CONTINUE
C
C    TRI is twice the area of the triangle element
C
      TRI=DXP(2)*DYP(3)-DXP(3)*DYP(2)+DXP(3)*DYP(1)-DXP(1)*DYP(3)
     :                +DXP(1)*DYP(2)-DXP(2)*DYP(1)
      ASUM = ASUM + TRI

      NNN=1
      DO 1 I = 1,4
      N1(I) = I + 2
 1    N2(I) = I
      GOTO (2,3,4) NNN
 3    N2(3) = 1
      N2(4) = 2
      GOTO 2
 4    N1(1) = 5
      N1(2) = 6
 2    DO 5 I = 1,4
      X1(I) = Y1(N1(I)) - Y1(N2(I))
 5    X2(I) = Y2(N1(I)) - Y2(N2(I))
C
C     CALCULATE ELEMENTS OF STRAIN TENSOR
C
C       Calculated as:
C       F(1) = Exx = du/dx+1
C       F(2) = Exy = du/dy
C       F(3) = Eyx = dv/dx
C       F(4) = Eyy = dv/dy+1
C       If there is no deformation, strain ellipse is a circle with
C       radius of 1 and F(1) = F(4) = 1
      DET = X1(1)*X1(4) - X1(2)*X1(3)
C       update sum over the elements
      DFAVG(1) = (X2(1)*X1(4) - X1(2)*X2(3))/DET
      DFAVG(2) = (X1(1)*X2(3) - X2(1)*X1(3))/DET
      DFAVG(3) = (X2(2)*X1(4) - X1(2)*X2(4))/DET
      DFAVG(4) = (X1(1)*X2(4) - X2(2)*X1(3))/DET
      DFAVGSUM(1) = DFAVGSUM(1) + DFAVG(1) * TRI
      DFAVGSUM(2) = DFAVGSUM(2) + DFAVG(2) * TRI
      DFAVGSUM(3) = DFAVGSUM(3) + DFAVG(3) * TRI
      DFAVGSUM(4) = DFAVGSUM(4) + DFAVG(4) * TRI
 
C     CALCULATE ORIENTATION OF PRINCIPAL AXES
C
C       AL1 and AL2 are STRAIN "in plane" MIN and MAX
C       AL3 is STRAIN vertical
C
      BOT = DFAVG(1)*DFAVG(1) + DFAVG(3)*DFAVG(3)
     :       - DFAVG(2)*DFAVG(2) - DFAVG(4)*DFAVG(4)
      TOP = 2.*(DFAVG(1)*DFAVG(2) + DFAVG(3)*DFAVG(4))
      THETA1 = PI/2
      IF(ABS(BOT).GT.1.E-37) THETA1 = 0.5*ATAN(TOP/BOT)
      C = COS(THETA1)
      S = SIN(THETA1)
      AL1 = SQRT((DFAVG(1)*C + DFAVG(2)*S)**2
     :             + (DFAVG(3)*C + DFAVG(4)*S)**2)
C       sum over the elements
      AL1SUM = AL1SUM + AL1 * TRI
      TOP = DFAVG(3)*C + DFAVG(4)*S
      BOT = DFAVG(1)*C + DFAVG(2)*S
      THETA2 = THETA1 + PI/2
      C = COS(THETA2)
      S = SIN(THETA2)
      AL2 = SQRT((DFAVG(1)*C + DFAVG(2)*S)**2
     :             + (DFAVG(3)*C + DFAVG(4)*S)**2)
      AL2SUM = AL2SUM + AL2 * TRI
      AL3 = 1.0/(AL1*AL2)
C
C     Calculate orientation of principal axes after deformation
C
      TH1 = PI/2
      IF(ABS(BOT).GT.1.E-37) TH1 = ATAN(TOP/BOT)
      TH2 = TH1 + PI/2
      ROT = TH1 - THETA1
      IF(AL2.GT.AL1)DEF=ALOG10(AL2/AL1)
      IF(AL2.LE.AL1)DEF=ALOG10(AL1/AL2)
      NC=2*(N-1) + 1
      WORK(NC)=DEF
      WORK(NC+1)=ROT*180.0/PI
      IF(IM.GE.3)GO TO 60
      XO = (Y2(1) + Y2(3) + Y2(5))/3
      YO = (Y2(2) + Y2(4) + Y2(6))/3
      IF (IVERBOSE.NE.0) THEN
        TH1DEG=TH1*180.0/PI
        WRITE(6,10555)N,XO,YO,AL1,AL2,AL3,TH1DEG
10555   FORMAT(I6,6G13.5)
      END IF
   60 CONTINUE
      IF (ASUM.NE.0.0) THEN
C
C       Average over the polygon elements
C         If using the convention that no deformation is
C         indicated by a strain of zero, subtract 1
C         This will give values greater than zero if the
C         polygon has been deformed, otherwise zero
        AL1AVG = AL1SUM/ASUM - 1
        AL2AVG = AL2SUM/ASUM - 1
        AL = MAX(AL2AVG,AL1AVG)
        FAVG(1) = DFAVGSUM(1)/ASUM - 1
        FAVG(2) = DFAVGSUM(2)/ASUM
        FAVG(3) = DFAVGSUM(3)/ASUM
        FAVG(4) = DFAVGSUM(4)/ASUM - 1
      ENDIF
      RETURN
      END

      SUBROUTINE STRAINPNT(IM,XPT,YPT,EX,EY,EXREF,EYREF,
     :                     LEM,NOR,IPOLYNUM,
     :                     FAVG,AL,NE,NUP,IVERBOSE,IERR)
C
C     STRAINAVG calculates an area averaged deformation for
C     the mesh element which contains XPT,YPT.
C     Only valid for small strain increments.
C     (TRI is twice element area)
C
C
C     Y1 CONTAINS COORDINATES OF UNDEFORMED ELEMENT
C
C     Y2 CONTAINS COORDINATES OF DEFORMED   ELEMENT
C
C     ORDER OF COORDINATES: X(PT1),Y(PT1), ....... ,X(PT3),Y(PT3)
C
C     NNN ALLOWS CHOICE OF RELATIVE POSITION VECTORS TO
C
C     BE PLACED IN X1 AND X2
C
C     NNN = 1:      (PT2 - PT1, PT3 - PT2) (PT2 is reference pt)
C
C     NNN = 2:      (PT2 - PT1, PT3 - PT1) (PT1 is reference pt)
C
C     NNN = 3:      (PT3 - PT2, PT3 - PT1) (PT3 is reference pt)
C
C
      DIMENSION EX(NUP),EY(NUP),EXREF(NUP),EYREF(NUP)
      DIMENSION LEM(6,NE),NOR(NUP)
      DIMENSION FAVG(4),IPOLYNUM(NE)
C     COMMON/CONTRO/LUW,LRBAT,LRINT,LDIN,LWINT,LPLOT,IAUTO,ILABEL
C    1 ,ICOLO
      DIMENSION X1(4),X2(4),F(4),N1(4),N2(4)
      DOUBLE PRECISION Y1(6),Y2(6),PNT(2),XPT,YPT
C     DIMENSION Y1(6),Y2(6),PNT(2)
C     DOUBLE PRECISION XPT,YPT
      DOUBLE PRECISION TRI,DXP(3),DYP(3),ASUM
      DOUBLE PRECISION DFAVG(4), DFAVGSUM(4)
      DATA PI /3.14159/
C
      IERR=0
      ASUM = 0.0
      AL1SUM = 0.0
      AL2SUM = 0.0
      DO 10 N=1,4
  10  DFAVGSUM(N) = 0.0
      DO 15 N=1,6
  15  Y1(N) = 0.0
      PNT(1) = XPT
      PNT(2) = YPT

C
C    For the triangle containing the point (first, if it lies
C     on a boundary
C

      DO 60 N=1,NE
C     IF (IPOLYNUM(N).NE.IM) GO TO 60
C
C    Put the element coordinates into the Y1 and Y2 matrices
C

      DO 20 K=1,3
      LK=NOR(LEM(K,N))
      JJ=2*(K-1)+1
      Y1(JJ)=EXREF(LK)
      Y1(JJ+1)=EYREF(LK)
      Y2(JJ)=EX(LK)
      Y2(JJ+1)=EY(LK)
      DXP(K)=EX(LK)
      DYP(K)=EY(LK)
   20 CONTINUE
      CALL CROSSINGSTEST(Y1,3,PNT,INSIDE)
      IF (INSIDE.EQ.0) GO TO 60
C        WRITE(6,10550)N,XPT,YPT
C10550   FORMAT('element',I4,' contains point ',2G13.5)
C
C    TRI is twice the area of the triangle element
C
      TRI=DXP(2)*DYP(3)-DXP(3)*DYP(2)+DXP(3)*DYP(1)-DXP(1)*DYP(3)
     :                +DXP(1)*DYP(2)-DXP(2)*DYP(1)
      ASUM = ASUM + TRI

      NNN=1
      DO 1 I = 1,4
      N1(I) = I + 2
 1    N2(I) = I
      GOTO (2,3,4) NNN
 3    N2(3) = 1
      N2(4) = 2
      GOTO 2
 4    N1(1) = 5
      N1(2) = 6
 2    DO 5 I = 1,4
      X1(I) = Y1(N1(I)) - Y1(N2(I))
 5    X2(I) = Y2(N1(I)) - Y2(N2(I))
C
C     CALCULATE ELEMENTS OF STRAIN TENSOR
C
C       Calculated as:
C       F(1) = Exx = du/dx+1
C       F(2) = Exy = du/dy
C       F(3) = Eyx = dv/dx
C       F(4) = Eyy = dv/dy+1
C       If there is no deformation, strain ellipse is a circle with
C       radius of 1 and F(1) = F(4) = 1
      DET = X1(1)*X1(4) - X1(2)*X1(3)
C       update sum over the elements
      DFAVG(1) = (X2(1)*X1(4) - X1(2)*X2(3))/DET
      DFAVG(2) = (X1(1)*X2(3) - X2(1)*X1(3))/DET
      DFAVG(3) = (X2(2)*X1(4) - X1(2)*X2(4))/DET
      DFAVG(4) = (X1(1)*X2(4) - X2(2)*X1(3))/DET
      DFAVGSUM(1) = DFAVGSUM(1) + DFAVG(1) * TRI
      DFAVGSUM(2) = DFAVGSUM(2) + DFAVG(2) * TRI
      DFAVGSUM(3) = DFAVGSUM(3) + DFAVG(3) * TRI
      DFAVGSUM(4) = DFAVGSUM(4) + DFAVG(4) * TRI
 
C     CALCULATE ORIENTATION OF PRINCIPAL AXES
C
C       AL1 and AL2 are STRAIN "in plane" MIN and MAX
C       AL3 is STRAIN vertical
C
      BOT = DFAVG(1)*DFAVG(1) + DFAVG(3)*DFAVG(3)
     :       - DFAVG(2)*DFAVG(2) - DFAVG(4)*DFAVG(4)
      TOP = 2.*(DFAVG(1)*DFAVG(2) + DFAVG(3)*DFAVG(4))
      THETA1 = PI/2
      IF(ABS(BOT).GT.1.E-37) THETA1 = 0.5*ATAN(TOP/BOT)
      C = COS(THETA1)
      S = SIN(THETA1)
      AL1 = SQRT((DFAVG(1)*C + DFAVG(2)*S)**2
     :             + (DFAVG(3)*C + DFAVG(4)*S)**2)
C       sum over the elements
      AL1SUM = AL1SUM + AL1 * TRI
      TOP = DFAVG(3)*C + DFAVG(4)*S
      BOT = DFAVG(1)*C + DFAVG(2)*S
      THETA2 = THETA1 + PI/2
      C = COS(THETA2)
      S = SIN(THETA2)
      AL2 = SQRT((DFAVG(1)*C + DFAVG(2)*S)**2
     :             + (DFAVG(3)*C + DFAVG(4)*S)**2)
      AL2SUM = AL2SUM + AL2 * TRI
      AL3 = 1.0/(AL1*AL2)
C
C     Calculate orientation of principal axes after deformation
C
      TH1 = PI/2
      IF(ABS(BOT).GT.1.E-37) TH1 = ATAN(TOP/BOT)
      TH2 = TH1 + PI/2
      ROT = TH1 - THETA1
      IF(AL2.GT.AL1)DEF=ALOG10(AL2/AL1)
      IF(AL2.LE.AL1)DEF=ALOG10(AL1/AL2)
      NC=2*(N-1) + 1
      XO = (Y2(1) + Y2(3) + Y2(5))/3
      YO = (Y2(2) + Y2(4) + Y2(6))/3
      IF (IVERBOSE.NE.0) THEN
        TH1DEG=TH1*180.0/PI
        WRITE(6,10555)N,XO,YO,AL1,AL2,AL3,TH1DEG
10555   FORMAT(I6,6G13.5)
      END IF
      GO TO 61
   60 CONTINUE
   61 IF (ASUM.NE.0.0) THEN
C
C       Average over the polygon elements
C         If using the convention that no deformation is
C         indicated by a strain of zero, subtract 1
C         This will give values greater than zero if the
C         polygon has been deformed, otherwise zero
        AL1AVG = AL1SUM/ASUM - 1
        AL2AVG = AL2SUM/ASUM - 1
        AL = MAX(AL2AVG,AL1AVG)
        FAVG(1) = DFAVGSUM(1)/ASUM - 1
        FAVG(2) = DFAVGSUM(2)/ASUM
        FAVG(3) = DFAVGSUM(3)/ASUM
        FAVG(4) = DFAVGSUM(4)/ASUM - 1
      ENDIF
      IF (INSIDE.EQ.0) THEN
        WRITE(6,10556)XPT,YPT
10556   FORMAT('No element contains point ',2G15.6)
        IERR=1
      END IF
      RETURN
      END

