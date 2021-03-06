C*--------------------------------------------------------------------
C*    Basil / Sybil:   strain.f  1.1  1 October 1998
C*
C*    Copyright (c) 1997 by G.A. Houseman, T.D. Barr, & L.A. Evans
C*    See README file for copying and redistribution conditions.
C*--------------------------------------------------------------------
C
      SUBROUTINE STRAIN(JELL,IQU,AMESH,BMESH,CMESH,DMESH,EMESH,
     1                  SE,BIG,EX,EY,VHB,UVP,LEM,NOR,
     2                  NE,NUP,NUVP,NP3,IVIS)
C
C    Routine to calculate different components of the strain or
C    stress tensor (or derived quantities) from the velocity field
C
      DIMENSION AMESH(NUP),BMESH(NUP),CMESH(NUP),DMESH(NUP)
      DIMENSION EMESH(NUP)
      DIMENSION EX(NUP),EY(NUP),VHB(8,NE),UVP(NUVP)
      DIMENSION LEM(6,NE),NOR(NUP)
      DIMENSION BY(3),CX(3),DNDP(84)
      DIMENSION PNI(42)
      DOUBLE PRECISION TRI,DPX2,DPX3,DPY2,DPY3

      EPS=1.0E-4
      PI=3.141592653
C
C    Zero the mesh first
C
      DO 10 I=1,NUP
      EMESH(I)=0.0
      AMESH(I)=0.0
      BMESH(I)=0.0
      CMESH(I)=0.0
   10 DMESH(I)=0.0

      SEXP=SE
C    bigv should be the same as that calculated in VISK (cginit.f)
      BIGV = BIG/10000.0
C
C    Look at each element in turn
C
      DO 50 N=1,NE
C
C     Calculate the geometrical coefficients
C
      TRI=0.0
      DO 20 K=1,3
      K2=MOD(K,3)+1
      K3=MOD(K+1,3)+1
      LK2=NOR(LEM(K2,N))
      LK3=NOR(LEM(K3,N))
      X2=EX(LK2)
      X3=EX(LK3)
      Y2=EY(LK2)
      Y3=EY(LK3)
      BY(K)=Y2 - Y3
      CX(K)=X3 - X2
      DPX2=X2
      DPX3=X3
      DPY2=Y2
      DPY3=Y3
      TRI=TRI + (DPX2*DPY3 - DPX3*DPY2)
   20 CONTINUE
C
C    TRI is twice the area of the triangle element
C    Get the derivatives of the interpolation function at the nodes
C
      CALL DNCOM(5,CX,BY,DNDP,PNI)
C
C     Find the internal angle made by the boundaries of the
C     triangle at this node
C
      DO 50 K=1,7
      IF (IVIS.EQ.0) THEN
        VF=1
      ELSE
        VF=VHB(K,N)
        SEXP=VHB(8,N)
      END IF
      IF(K.LE.6)THEN
        LK=LEM(K,N)
        IF(LK.LT.0)LK=-LK
        ANGL=0.5
      END IF
      IF(K.LE.3)THEN
        K2=MOD(K,3)+1
        K3=MOD(K+1,3)+1
        DB2= BY(K2)*BY(K2) + CX(K2)*CX(K2)
        DA2= BY(K3)*BY(K3) + CX(K3)*CX(K3)
        IF (DB2.EQ.0.0.OR.DA2.EQ.0.0) THEN
          SANGL=TRI/EPS
        ELSE
          SANGL=TRI/SQRT(DB2*DA2)
        END IF
        CSANGL=1.0-SANGL*SANGL
        IF(CSANGL.LT.0.)CSANGL=0.0
        CSANGL=SQRT(CSANGL)
        IF (SANGL.EQ.0.0.AND.CSANGL.EQ.0.0) THEN
          THETA = 0.0
        ELSE
          ANGL=0.5*ATAN2(SANGL,CSANGL)/PI
        END IF
      END IF
C
C    Location of node K
C
      IF(K.LE.3)THEN
        ILK=NOR(LEM(K,N))
        XP=EX(ILK)
        YP=EY(ILK)
      ELSE IF((K.GE.4).AND.(K.LE.6))THEN
        KK1=MOD(K+1,3)+1
        KK2=MOD(K+2,3)+1
        ILK1=NOR(LEM(KK1,N))
        ILK2=NOR(LEM(KK2,N))
        XP=0.5*(EX(ILK1)+EX(ILK2))
        YP=0.5*(EY(ILK1)+EY(ILK2))
      ELSE
        ILK1=NOR(LEM(1,N))
        ILK2=NOR(LEM(2,N))
        ILK3=NOR(LEM(3,N))
        XP=(EX(ILK1)+EX(ILK2)+EX(ILK3))/3.0
        YP=(EY(ILK1)+EY(ILK2)+EY(ILK3))/3.0
      END IF
C
C   Calculate the velocity derivatives at node K
C
   25 DUDX=0.0
      DUDY=0.0
      DVDX=0.0
      DVDY=0.0
C
C    Sum the interpolation functions
C
      DO 55 I=1,6
        NI=LEM(I,N)
        IF(NI.LT.0)NI=-NI
        KIN=(I-1)*14 + (K-1)*2 + 1
        UI=UVP(NI)
        VI=UVP(NI+NUP)
C
C    du/dx, du/dy, dv/dx, dv/dy at node K
C
        DUDX=DUDX + UI*DNDP(KIN)
        DUDY=DUDY + UI*DNDP(KIN+1)
        DVDX=DVDX + VI*DNDP(KIN)
        DVDY=DVDY + VI*DNDP(KIN+1)
   55 CONTINUE
C     IF (TRI.EQ.0.0) TRI=EPS
      DUDX=DUDX/TRI
      DVDX=DVDX/TRI
      DUDY=DUDY/TRI
      DVDY=DVDY/TRI
C     ENDIF
      DWDZ=-DUDX-DVDY
C
C    Following section calculates quanties:
C      EDXX...BRIT
C
      EDXX=DUDX
      EDYY=DVDY
      EDZZ=DWDZ
      EDXY=0.5*(DUDY+DVDX)
      VORT=DVDX-DUDY
      ED2I=SQRT(EDXX*EDXX + EDYY*EDYY + EDZZ*EDZZ + 2.0*EDXY*EDXY)
      IF(ED2I.GT.0.0)THEN
         VISC = VF*ED2I**(1.0/SEXP - 1.0)
         IF(VISC.GT.BIGV) VISC = BIGV
         THDI = VF*ED2I**(1.0/SEXP + 1.0)
      ELSE
         VISC = BIGV
         THDI=0.0
      END IF
C
C    principal strain rates and stresses
C      THETA and THETA1 are the angles of the principal stress axes
C
      IF ((DVDX+DUDY).EQ.0.0.AND.(DUDX-DVDY).EQ.0.0) THEN
        THETA = 0.0
      ELSE
        THETA=0.5*ATAN2((DVDX+DUDY),(DUDX-DVDY))
      END IF
      THETA1=THETA+PI*0.5
      TCOS=COS(THETA)
      TCOS1=COS(THETA1)
      TSIN=SIN(THETA)
      TSIN1=SIN(THETA1)
      S01=EDXX*TCOS*TCOS + EDYY*TSIN*TSIN + 2.0*EDXY*TSIN*TCOS
      S02=EDXX*TCOS1*TCOS1 + EDYY*TSIN1*TSIN1 + 2.0*EDXY*TSIN1*TCOS1
      IF(S01.GE.S02)THEN
         PSR1=S01
         PSR2=S02
         CANG=THETA
         TANG=THETA1
      ELSE
         PSR1=S02
         PSR2=S01
         CANG=THETA1
         TANG=THETA
      END IF
      PSRM=0.5*(PSR1-PSR2)
      SANG=0.5*(CANG+TANG)
      IF (IQU.EQ.24) VOTA=VORT/(PSR1-PSR2)
      TAXX=2.0*VISC*EDXX
      TAYY=2.0*VISC*EDYY
      TAZZ=2.0*VISC*EDZZ
      TAXY=2.0*VISC*EDXY
      TAU1=2.0*VISC*PSR1
      TAU2=2.0*VISC*PSR2
      TAUM=2.0*VISC*PSRM
C
C    Type of faulting : sum of two double couples :
C       0 to 1/4 : thrust + thrust ; 1/4 to 1/2 : thrust + strike slip ;
C       1/2 to 3/4 : normal + strike slip ; 3/4 to 1 : normal + normal
C       Note:(1/4 to 3/8 thrust dominates strike slip faulting
C             3/8 to 1/2 strike slip dominates thrust faulting
C             1/2 to 5/8 strike slip dominates normal faulting)
C             5/8 to 3/4 normal dominates strike slip faulting
C
      IF (PSR1.NE.0.0) DBLC=(0.75*PI+ATAN(PSR2/PSR1))/PI
      IF(PSR1.LT.0.0) DBLC=DBLC-1.0
C
C    Pressure below which is brittle failure (must subtract pressure term)
C      AMU is the internal coefficient of friction
C
      AMU=0.85
      SAVG=0.5*(PSR1+PSR2)
      APSS= -PSRM*SQRT(1.0+1.0/AMU/AMU)-SAVG
C Normal faulting for thin viscous sheet case
      TMAX=0.5*(PSR1-EDZZ)
      SAVG=0.5*(PSR1+EDZZ)
      APN= -TMAX*SQRT(1.0+1.0/AMU/AMU)-SAVG
C Thrust faulting for thin viscous sheet case
      TMAX=0.5*(EDZZ-PSR2)
      SAVG=0.5*(EDZZ+PSR2)
      APT= -TMAX*SQRT(1.0+1.0/AMU/AMU)-SAVG
C Take the minimum of APSS, APN and APT
        IF(APSS.LE.APN.AND.APSS.LE.APT) TI=APSS
        IF(APN.LE.APSS.AND.APN.LE.APT) TI=APN
        IF(APT.LE.APSS.AND.APT.LE.APN) TI=APT
      BRIT=2.0*VISC*TI
C
C Type of faulting (SS=0.5, N=-0.5, T=1.5)
C    (Contour with level=0.0 and interval=1.0)
C
       IF(APN.LE.APSS.AND.APN.LE.APT) TI= -0.5
       IF(APT.LE.APSS.AND.APT.LE.APN) TI=1.5
       IF(APSS.LE.APN.AND.APSS.LE.APT) TI=0.5
      BRI2=TI
C
C  Orientation of the intermediate deviatoric stress
C   Vertical=0.5, PSR1=-0.5, PSR2=1.5
C    (Contour with level=0.0 and interval=1.0)
C    
C     FOLT=0.5
C     IF((EDZZ.GT.PSR1).AND.(EDZZ.GT.PSR2)) FOLT=-0.5
C     IF((EDZZ.LT.PSR1).AND.(EDZZ.LT.PSR2)) FOLT=1.5
C
C  Pressure isn't calculated here since it requires a linear
C   interpolation instead of a quadratic one - therefore it is
C   subtracted after the NTRPLT step
C
      PRES=0.0
      SIG1=TAU1+PRES
      SIG2=TAU2+PRES
      SIXX=TAXX+PRES
      SIYY=TAYY+PRES
      SIZZ=TAZZ+PRES
      BRIT=BRIT-PRES
C
      IF(JELL.EQ.0.AND.K.LT.7) THEN
        SI=0.0
        IF(IQU.EQ.1)  SI=EDXX
        IF(IQU.EQ.2)  SI=EDYY
        IF(IQU.EQ.3)  SI=EDZZ
        IF(IQU.EQ.4)  SI=EDXY
        IF(IQU.EQ.5)  SI=PSR1
        IF(IQU.EQ.6)  SI=PSR2
        IF(IQU.EQ.7)  SI=PSRM
        IF(IQU.EQ.8)  SI=CANG
        IF(IQU.EQ.9)  SI=TANG
        IF(IQU.EQ.10) SI=SANG
        IF(IQU.EQ.11) SI=DBLC
        IF(IQU.EQ.12) SI=VORT
        IF(IQU.EQ.13) SI=ED2I
        IF(IQU.EQ.14) SI=THDI
        IF(IQU.EQ.15) SI=ALOG10(VISC)
        IF(IQU.EQ.16) SI=TAXX
        IF(IQU.EQ.17) SI=TAYY
        IF(IQU.EQ.18) SI=TAZZ
        IF(IQU.EQ.19) SI=TAXY
        IF(IQU.EQ.20) SI=TAU1
        IF(IQU.EQ.21) SI=TAU2
        IF(IQU.EQ.22) SI=TAUM
        IF(IQU.EQ.23) SI=0.0
        IF(IQU.EQ.24) SI=VOTA
        IF(IQU.EQ.25) SI=SIXX
        IF(IQU.EQ.26) SI=SIYY
        IF(IQU.EQ.27) SI=SIZZ
        IF(IQU.EQ.28) SI=SIG1
        IF(IQU.EQ.29) SI=SIG2
        IF(IQU.EQ.30) SI=PRES
        IF(IQU.EQ.31) SI=BRIT
        IF(IQU.EQ.32) SI=BRI2
C       IF(IQU.EQ.33) SI=FOLT
        AMESH(LK)=AMESH(LK) + SI*ANGL
        EMESH(LK)=EMESH(LK) + ANGL
C
      ELSE IF(JELL.EQ.1.AND.K.LT.7) THEN
C
        SI=0.0
C
C    Amplitude and direction of principal strain axes
C
        IF(IQU.EQ.1) THEN
          ARRO1=PSR1
          ARRO2=PSR2
          ARRANG=CANG
C         IF (ABS(ABS(PSR1)-ABS(PSR2)).LT.EPS) ARRANG=PI*0.25
        END IF
C
C     Direction only of principal strain axes
C
        IF(IQU.EQ.2) THEN
          ARRO1=1.0
          ARRO2=-1.0
          ARRANG=CANG
C         IF (ABS(ABS(PSR1)-ABS(PSR2)).LT.EPS) ARRANG=PI*0.25
        END IF
C
C     Amplitude and direction of principal stresses
C
        IF(IQU.EQ.3) THEN
          ARRO1=TAU1
          ARRO2=TAU2
          ARRANG=CANG
C         IF (ABS(ABS(PSR1)-ABS(PSR2)).LT.EPS) ARRANG=PI*0.25
        END IF
C
C     amplitude and direction of maximum shear strain rate
C
        IF(IQU.EQ.4) THEN
          ARRO1=PSRM
          ARRO2=-PSRM
          ARRANG=CANG+PI*0.25
C         IF (ABS(ABS(PSR1)-ABS(PSR2)).LT.EPS) ARRANG=PI*0.5
        END IF
C
C     Direction only of maximum shear stresses
C
        IF(IQU.EQ.5) THEN
          ARRO1=1.0
          ARRO2=-1.0
          ARRANG=CANG+PI*0.25
C         IF (ABS(ABS(PSR1)-ABS(PSR2)).LT.EPS) ARRANG=PI*0.5
        END IF
C
C     Direction and magnitude of maximum shear stress
C
        IF(IQU.EQ.22) THEN
          ARRO1=TAUM
          ARRO2=-TAUM
          ARRANG=CANG+PI*0.25
C         IF (ABS(ABS(PSR1)-ABS(PSR2)).LT.EPS) ARRANG=PI*0.5
        END IF
C
C     Direction of likely strike-slip faulting 
C
        IF(IQU.EQ.7) THEN
          ARRO1=1.0
          ARRO2=0.0
          ARRANG=CANG+PI/3.0
          CAT=COS(ARRANG)
          SAT=SIN(ARRANG)
          ROTRA1=(DVDY-DUDX)*SAT*CAT + DVDX*CAT*CAT - DUDY*SAT*SAT
          ARRANG=CANG+PI*2.0/3.0
          CAT=COS(ARRANG)
          SAT=SIN(ARRANG)
          ROTRA2=(DVDY-DUDX)*SAT*CAT + DVDX*CAT*CAT - DUDY*SAT*SAT
          IF(ABS(ROTRA1).LE.ABS(ROTRA2))THEN
             ARRANG=CANG+PI/3.0
          ELSE
             ARRANG=CANG+PI*2.0/3.0
          END IF
        END IF
C
C   check for aliasing of the angle by averaging sin(2TH) and cos(2TH)
C    sines and cosines are transferred to ARWTWO, where theta is extracted
C
        AMESH(LK)=AMESH(LK) + COS(2.0*ARRANG+PI*0.5)*ANGL
        BMESH(LK)=BMESH(LK) + SIN(2.0*ARRANG+PI*0.5)*ANGL
C       NLK=LK+NUPP
        EMESH(LK)=EMESH(LK) + ANGL
        CMESH(LK)=CMESH(LK) + ARRO1*ANGL
        DMESH(LK)=DMESH(LK) + ARRO2*ANGL
      END IF
   50 CONTINUE
C
C    Normalise by the total angle around the node
C     note that averaging may produce odd effect on angles
C     in neighbourhood of reflection symmetry axis
C
      IF(JELL.EQ.0)THEN
        DO 60 I=1,NUP
          TANGL=EMESH(I)
          IF (TANGL.EQ.0.0) TANGL=EPS
          AMESH(I)=AMESH(I)/TANGL
   60   CONTINUE
      ELSE IF(JELL.EQ.1)THEN
        DO 70 I=1,NUP
          TANGL=EMESH(I)
          IF (TANGL.EQ.0.0) TANGL=EPS
          AMESH(I)=AMESH(I)/TANGL
          BMESH(I)=BMESH(I)/TANGL
          CMESH(I)=CMESH(I)/TANGL
          DMESH(I)=DMESH(I)/TANGL
   70   CONTINUE
      END IF
C      CALL MATPRT(AMESH,NUP,1,NUP)
      RETURN
C 200 WRITE(LUW,10102) JELL
C0102 FORMAT('INVALID VALUE OF JELL =',I4)
C     STOP
      END
C
      SUBROUTINE PAVG(SUM,EX,EY,UVP,NN,NUP,NFP,NUVP)
C
C  Routine to calculate the mean of the pressure since the pressure
C  is indeterminate by a constant amount
C
      DIMENSION EX(NUP),EY(NUP),UVP(NUVP)
C     INCLUDE 'cg.parameters'
C     COMMON/B1/EX1(NUPP)
C     COMMON/C1/EY1(NUPP)
C     COMMON/AD/UVP(NROWSP)
C     COMMON/CONTRO/LUW,LRBAT,LRINT,LDIN,LWINT,LPLOT,IAUTO,ILABEL,ICOLO
      SUM=0.0
      ITOT=0
      DO 10 I=1,NN
        AX=EX(I)-1.0
        AY=EY(I)-1.0
        RR=SQRT(AX*AX+AY*AY)
        NI=I+NUP+NUP+NFP
        IF(RR.GT.0.05.AND.RR.LT.0.5) THEN
           SUM=SUM+UVP(NI)
           ITOT=ITOT+1
        END IF
   10 CONTINUE
      IF (ITOT.NE.0) SUM=SUM/ITOT
C      WRITE(LSC,10001)SUM
C10001 FORMAT("The mean pressure is ",G12.5)
      RETURN
      END

      SUBROUTINE NTRPLTPRES(AMESH,PRES,NOR,LEM,
     :                      NUP,NE,NN)
      DIMENSION AMESH(NUP),NOR(NUP),PRES(NN),LEM(6,NE)

      DO 10 N = 1,NE
        DO 20 K = 1,3
          LK = LEM(K,N)
          NLK = NOR(LK)
          AMESH(LK) = PRES(NLK)
          K4 = MOD(K,3) + 3
          LK4 = LEM(K4,N)
          K3 = MOD(K+1,3) + 1
          LK3 = LEM(K3,N)
          NLK3 = NOR(LK3)
          AMESH(LK4) = 0.5 * (PRES(NLK) + PRES(NLK3))
  20    CONTINUE
  10  CONTINUE
      RETURN
      END
