C*--------------------------------------------------------------------
C*    Basil / Sybil:   dncom.f  1.1  1 October 1998
C*
C*    Copyright (c) 1997 by G.A. Houseman, T.D. Barr, & L.A. Evans
C*    See README file for copying and redistribution conditions.
C*--------------------------------------------------------------------
C
C      contains subroutines DNCOM, RANGXY, RSCALE, SWAPCO
C
      SUBROUTINE DNCOM(JJ,CX,BY,DNDP,PNI)
      DIMENSION ALFA(10)
C
C     This routine calculates the gradient of the interpolation
C     function N(I) at a location in the element determined by
C     the parameter J.  The calculation for X and Y gradient is
C     the same, though different geometrical elements in B are
C     used.  For numbering the points (I & J), vertices are 1-3
C     midpoints are 4-6, and the centroid (J only) is 7.  The
C     interpolation functions are given in Huebner (p345).
C
C     The interpolation function is for a vertex point
C
      DIMENSION BY(3),CX(3),DNDP(84)
      DIMENSION PNI(42)
      DATA ALFA/0.05971587,0.47014206,0.79742699,0.10128651,
     10.33333333,0.0,0.5,1.0,0.0,0.33333333/
      INP=0
      IND=0
      I=0
   18 I=I+1
      J=0
   19 J=J+1
      IF(J.LE.3)GO TO 11
      IF(J.LE.6)GO TO 13
      GO TO 15
C
C     J  is a vertex point
C
   11 IF(J.EQ.I)GO TO 12
      KN1=4
      KN2=4
      KN3=3
      GO TO 16
   12 KN1=3
      KN2=4
      KN3=4
      GO TO 16
C
C    J is a midpoint
C
   13 IX=MOD(I+1,3)+4
      IF(J.EQ.IX)GO TO 14
      KN1=2
      KN2=1
      KN3=2
      GO TO 16
   14 KN1=1
      KN2=2
      KN3=2
      GO TO 16
C
C     J is the centroid
C
   15 KN1=5
      KN2=5
      KN3=5
   16 I2=MOD(I,3)+1
      I3=MOD(I+1,3)+1
      CL1=ALFA(KN1+JJ)
      CL2=ALFA(KN2+JJ)
      CL3=ALFA(KN3+JJ)
      DNDP(IND+1)=BY(I)*(2.0*CL1-CL2-CL3)-CL1*(BY(I2)+BY(I3))
      DNDP(IND+2)=CX(I)*(2.0*CL1-CL2-CL3)-CL1*(CX(I2)+CX(I3))
      IND=IND+2
      INP=INP+1
      PNI(INP)=CL1*CL1 - CL1*(CL2+CL3)
      IF(J.LT.7)GO TO 19
      IF(I.LT.3)GO TO 18
C
C    The interpolation function is for a midpoint
C
      I=3
  220 I=I+1
      J=0
  221 J=J+1
      I1=MOD(I,3)+1
      I2=MOD(I1,3)+1
      I3=MOD(I1+1,3)+1
      IF(J.LE.3)GO TO 211
      IF(J.LE.6)GO TO 214
      GO TO 217
C
C    J is a vertex point
C
  211 IF(J.NE.I2)GO TO 212
      KN2=3
      KN3=4
      GO TO 218
  212 IF(J.NE.I3)GO TO 213
      KN2=4
      KN3=3
      GO TO 218
  213 KN2=4
      KN3=4
      GO TO 218
C
C    J is a mid-point node
C
  214 I5 = 4 + MOD(I,3)
      I6 = 4 + MOD(I+1,3)
      IF(J.NE.I5)GO TO 215
      KN2=1
      KN3=2
      GO TO 218
  215 IF(J.NE.I6)GO TO 216
      KN2=2
      KN3=1
      GO TO 218
  216 KN2=2
      KN3=2
      GO TO 218
C
C    J is the centroid
C
  217 KN2=5
      KN3=5
C
  218 CL2=ALFA(KN2+JJ)
      CL3=ALFA(KN3+JJ)
      DNDP(IND+1) = 4.0*(CL3*BY(I2) + CL2*BY(I3))
      DNDP(IND+2) = 4.0*(CL3*CX(I2) + CL2*CX(I3))
      IND=IND+2
      INP=INP+1
      PNI(INP)=4.0*CL2*CL3
      IF(J.LT.7)GO TO 221
      IF(I.LT.6)GO TO 220
      RETURN
      END

      SUBROUTINE RANGXY(X,IHELP,NXY,M1,M2,N1,N2,XMAX,IM,JM,
     1                  XMIN,IN,JN,NX3)
C
C    This routine determines Min and Max of an array to be contoured
C    It does not look at points for which IHELP=0 (outside the mesh)
C
      DIMENSION X(NXY),IHELP(NXY)
      IM1 = (N1 - 1)*NX3 + M1
      IM = M1
      IN = M1
      JM = N1
      JN = N1
      XMAX = X(IM1)
      XMIN = X(IM1)
      DO 1 I = M1,M2
      DO 1 J = N1,N2
      K = (J - 1)*NX3 + I
      IF(IHELP(K).NE.0)THEN
        IF(X(K).GT.XMAX)THEN
          XMAX = X(K)
          IM = I
          JM = J
        ELSE IF(X(K).LT.XMIN)THEN
          XMIN = X(K)
          IN = I
          JN = J
        END IF
      END IF
 1    CONTINUE
C     WRITE(LUW,10100)XMAX,IM,JM,XMIN,IN,JN
C10100 FORMAT(' MAX VALUE OF FUNCTION IS ',G12.5,' AT I =',I6,
C    1', J =',I6,'   MIN VALUE IS ',G12.5,' AT I =',I6,', J =',
C    2I6,/)
      RETURN
      END

      SUBROUTINE RSCALE(X,Y,NXY,M1,M2,N1,N2,RMAX,RMIN,NX3)
      DIMENSION X(NXY),Y(NXY)
      K=(N1-1)*NX3+M1
      RMAX=SQRT(X(K)*X(K)+Y(K)*Y(K))
      RMIN=RMAX
      DO 1 I = M1,M2
      DO 1 J = N1,N2
      K = (J - 1)*NX3 + I
      R = SQRT(X(K)*X(K) + Y(K)*Y(K))
      RMAX=MAX(R,RMAX)
      RMIN=MIN(R,RMIN)
 1    CONTINUE
      RETURN
      END

      SUBROUTINE SWAPCO(EX,EY,UVP,NUP,JROT,JVELROT)
C
C  routine to swap X and Y coordinates for purposes of
C   plotting solution
C
      DIMENSION EX(NUP),EY(NUP),UVP(NUP,2)
C
C    Rotate entire mesh and, if JVELROT=1, velocity field anti - clockwise
C    in steps of 90 degrees (JROT steps)
C
      PIB2=3.141592653/2.0
      DO 50 I=1,NUP
        Y=EY(I)
        X=EX(I)
        RD=SQRT(X*X + Y*Y)
        IF (X.EQ.0.0.AND.Y.EQ.0.0) THEN
          TD=0
        ELSE
          TD=ATAN2(Y,X)
        END IF
        TD=TD+PIB2*FLOAT(JROT)
        EX(I)=RD*COS(TD)
        EY(I)=RD*SIN(TD)
        IF (JVELROT.NE.0) THEN
          U=UVP(I,1)
          V=UVP(I,2)
          RD=SQRT(U*U + V*V)
          IF (U.EQ.0.0.AND.V.EQ.0.0) THEN
            TD=0
          ELSE
            TD=ATAN2(V,U)
          END IF
          TD=TD+PIB2*FLOAT(JROT)
          UVP(I,1)=RD*COS(TD)
          UVP(I,2)=RD*SIN(TD)
        END IF
   50 CONTINUE
      RETURN
      END

