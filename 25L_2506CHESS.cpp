//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include <string>
//#include <cmath>
//
//using namespace sf;
//using namespace std;
//
//// --- Types ---
//enum PieceType { NONE, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING };
//enum PieceColor { NO_COLOR, WHITE, BLACK };
//PieceType boardType[8][8];
//PieceColor boardColor[8][8];
//bool boardMoved[8][8];
//bool boardEnPassant[8][8];
//
//// active player
//PieceColor activeColor = WHITE;
//
//// helper
//bool inBounds(int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; }
//int sign(int x) { return (x > 0) - (x < 0); }
//
//// Textures
//Texture texWP, texWR, texWKN, texWB, texWQ, texWK;
//Texture texBP, texBR, texBKN, texBB, texBQ, texBK;
//
//// Forward declarations
//void setupInitialBoard();
//bool isSquareAttacked(int tr, int tc, PieceColor byColor);
//pair<int, int> findKing(PieceColor color);
//bool isInCheck(PieceColor color);
//bool isLegalMoveInternal(int sr, int sc, int er, int ec, PieceColor mover);
//bool wouldLeaveKingInCheck(int sr, int sc, int er, int ec);
//bool makeMoveIfLegal(int sr, int sc, int er, int ec);
//bool anyLegalMoveExists(PieceColor color);
//void clearAllEnPassantFlags();
//string pieceName(PieceType t, PieceColor c);
//Texture* textureFor(PieceType t, PieceColor c);
//
//// --- captured-highlight (red) bookkeeping ---
//int lastCapR = -1, lastCapC = -1;
//PieceType lastCapType = NONE;
//PieceColor lastCapColor = NO_COLOR;
//int capTimer = 0;
//const int CAP_FRAMES = 30; // frames to show last-capture
//
//// --- Implementation ---
//void setupInitialBoard() {
//    for (int r = 0; r < 8; r++)
//        for (int c = 0; c < 8; c++) {
//            boardType[r][c] = NONE;
//            boardColor[r][c] = NO_COLOR;
//            boardMoved[r][c] = false;
//            boardEnPassant[r][c] = false;
//        }
//
//    boardType[7][0] = ROOK; boardColor[7][0] = WHITE;
//    boardType[7][1] = KNIGHT; boardColor[7][1] = WHITE;
//    boardType[7][2] = BISHOP; boardColor[7][2] = WHITE;
//    boardType[7][3] = QUEEN; boardColor[7][3] = WHITE;
//    boardType[7][4] = KING; boardColor[7][4] = WHITE;
//    boardType[7][5] = BISHOP; boardColor[7][5] = WHITE;
//    boardType[7][6] = KNIGHT; boardColor[7][6] = WHITE;
//    boardType[7][7] = ROOK; boardColor[7][7] = WHITE;
//    for (int c = 0; c < 8; c++) { boardType[6][c] = PAWN; boardColor[6][c] = WHITE; }
//
//    boardType[0][0] = ROOK; boardColor[0][0] = BLACK;
//    boardType[0][1] = KNIGHT; boardColor[0][1] = BLACK;
//    boardType[0][2] = BISHOP; boardColor[0][2] = BLACK;
//    boardType[0][3] = QUEEN; boardColor[0][3] = BLACK;
//    boardType[0][4] = KING; boardColor[0][4] = BLACK;
//    boardType[0][5] = BISHOP; boardColor[0][5] = BLACK;
//    boardType[0][6] = KNIGHT; boardColor[0][6] = BLACK;
//    boardType[0][7] = ROOK; boardColor[0][7] = BLACK;
//    for (int c = 0; c < 8; c++) { boardType[1][c] = PAWN; boardColor[1][c] = BLACK; }
//
//    activeColor = WHITE;
//
//    lastCapR = lastCapC = -1;
//    lastCapType = NONE; lastCapColor = NO_COLOR; capTimer = 0;
//}
//
//pair<int, int> findKing(PieceColor color) {
//    for (int r = 0; r < 8; r++)
//        for (int c = 0; c < 8; c++)
//            if (boardType[r][c] == KING && boardColor[r][c] == color) return { r,c };
//    return { -1,-1 };
//}
//
//bool isSquareAttacked(int tr, int tc, PieceColor byColor) {
//    if (byColor == NO_COLOR) return false;
//
//    // A white pawn that attacks (tr,tc) must be at row tr + 1.
//    int pawnDir = (byColor == WHITE) ? 1 : -1;
//    int pr = tr + pawnDir;
//    if (inBounds(pr, tc - 1) && boardType[pr][tc - 1] == PAWN && boardColor[pr][tc - 1] == byColor) return true;
//    if (inBounds(pr, tc + 1) && boardType[pr][tc + 1] == PAWN && boardColor[pr][tc + 1] == byColor) return true;
//
//    const int kdr[8] = { 2,2,1,1,-1,-1,-2,-2 };
//    const int kdc[8] = { 1,-1,2,-2,2,-2,1,-1 };
//    for (int i = 0; i < 8; i++) {
//        int r = tr + kdr[i], c = tc + kdc[i];
//        if (inBounds(r, c) && boardType[r][c] == KNIGHT && boardColor[r][c] == byColor) return true;
//    }
//
//    const int ddr[4] = { 1,1,-1,-1 };
//    const int ddc[4] = { 1,-1,1,-1 };
//    for (int d = 0; d < 4; d++) {
//        int r = tr + ddr[d], c = tc + ddc[d];
//        while (inBounds(r, c)) {
//            if (boardType[r][c] != NONE) {
//                if (boardColor[r][c] == byColor && (boardType[r][c] == BISHOP || boardType[r][c] == QUEEN)) return true;
//                break;
//            }
//            r += ddr[d]; c += ddc[d];
//        }
//    }
//
//    const int odr[4] = { 1,-1,0,0 };
//    const int odc[4] = { 0,0,1,-1 };
//    for (int d = 0; d < 4; d++) {
//        int r = tr + odr[d], c = tc + odc[d];
//        while (inBounds(r, c)) {
//            if (boardType[r][c] != NONE) {
//                if (boardColor[r][c] == byColor && (boardType[r][c] == ROOK || boardType[r][c] == QUEEN)) return true;
//                break;
//            }
//            r += odr[d]; c += odc[d];
//        }
//    }
//
//    for (int dr = -1; dr <= 1; dr++) for (int dc = -1; dc <= 1; dc++) {
//        if (dr == 0 && dc == 0) continue;
//        int r = tr + dr, c = tc + dc;
//        if (inBounds(r, c) && boardType[r][c] == KING && boardColor[r][c] == byColor) return true;
//    }
//
//    return false;
//}
//
//bool isInCheck(PieceColor color) {
//    pair<int, int> K = findKing(color);
//    if (K.first == -1) return false;
//    return isSquareAttacked(K.first, K.second, (color == WHITE) ? BLACK : WHITE);
//}
//
//bool isLegalMoveInternal(int sr, int sc, int er, int ec, PieceColor mover) {
//    if (!inBounds(sr, sc) || !inBounds(er, ec)) return false;
//    PieceType pType = boardType[sr][sc];
//    PieceColor pColor = boardColor[sr][sc];
//    bool pMoved = boardMoved[sr][sc];
//
//    if (pType == NONE || pColor != mover) return false;
//    if (sr == er && sc == ec) return false;
//    PieceType destType = boardType[er][ec];
//    PieceColor destColor = boardColor[er][ec];
//    if (destType != NONE && destColor == mover) return false;
//
//    int dr = er - sr;
//    int dc = ec - sc;
//
//    switch (pType) {
//    case PAWN: {
//        int dir = (pColor == WHITE) ? -1 : 1;
//        if (dc == 0 && dr == dir && boardType[er][ec] == NONE) return true;
//        if (dc == 0 && dr == 2 * dir && !pMoved) {
//            int midr = sr + dir;
//            if (boardType[midr][sc] == NONE && boardType[er][ec] == NONE) return true;
//        }
//        if (abs(dc) == 1 && dr == dir) {
//            if (boardType[er][ec] != NONE && boardColor[er][ec] != mover) return true;
//            if (inBounds(sr, ec) && boardType[sr][ec] == PAWN && boardColor[sr][ec] != mover && boardEnPassant[sr][ec]) return true;
//        }
//        return false;
//    }
//    case KNIGHT: return (abs(dr) == 2 && abs(dc) == 1) || (abs(dr) == 1 && abs(dc) == 2);
//    case BISHOP:
//        if (abs(dr) != abs(dc)) return false;
//        for (int r = sr + sign(dr), c = sc + sign(dc); r != er; r += sign(dr), c += sign(dc))
//            if (boardType[r][c] != NONE) return false;
//        return true;
//    case ROOK:
//        if (dr != 0 && dc != 0) return false;
//        for (int r = sr + (dr != 0 ? sign(dr) : 0), c = sc + (dc != 0 ? sign(dc) : 0); r != er || c != ec; r += (dr != 0 ? sign(dr) : 0), c += (dc != 0 ? sign(dc) : 0))
//            if (boardType[r][c] != NONE) return false;
//        return true;
//    case QUEEN:
//        if (dr == 0 || dc == 0 || abs(dr) == abs(dc)) {
//            for (int r = sr + (dr != 0 ? sign(dr) : 0), c = sc + (dc != 0 ? sign(dc) : 0); r != er || c != ec; r += (dr != 0 ? sign(dr) : 0), c += (dc != 0 ? sign(dc) : 0))
//                if (boardType[r][c] != NONE) return false;
//            return true;
//        }
//        return false;
//    case KING:
//        if (abs(dr) <= 1 && abs(dc) <= 1) return true;
//        if (!pMoved && dr == 0 && abs(dc) == 2) {
//            int rookCol = (dc > 0) ? 7 : 0;
//            if (boardType[sr][rookCol] == ROOK && boardColor[sr][rookCol] == mover && !boardMoved[sr][rookCol]) {
//                int dirc = (dc > 0) ? 1 : -1;
//                for (int c = sc + dirc; c != rookCol; c += dirc) if (boardType[sr][c] != NONE) return false;
//                // ensure not castling out of, through, or into check
//                PieceColor opp = (mover == WHITE) ? BLACK : WHITE;
//                if (isSquareAttacked(sr, sc, opp)) return false;
//                if (isSquareAttacked(sr, sc + dirc, opp)) return false;
//                if (isSquareAttacked(sr, sc + 2 * dirc, opp)) return false;
//                return true;
//            }
//        }
//        return false;
//    default: return false;
//    }
//}
//
//bool wouldLeaveKingInCheck(int sr, int sc, int er, int ec) {
//    // backup source
//    PieceType backupSrcType = boardType[sr][sc];
//    PieceColor backupSrcColor = boardColor[sr][sc];
//    bool backupSrcMoved = boardMoved[sr][sc];
//    bool backupSrcEP = boardEnPassant[sr][sc];
//
//    // backup destination
//    PieceType backupDstType = boardType[er][ec];
//    PieceColor backupDstColor = boardColor[er][ec];
//    bool backupDstMoved = boardMoved[er][ec];
//    bool backupDstEP = boardEnPassant[er][ec];
//
//    bool capturedEP = false;
//    PieceType capturedEPBackupType = NONE;
//    PieceColor capturedEPBackupColor = NO_COLOR;
//    bool capturedEPBackupMoved = false;
//    bool capturedEPBackupEP = false;
//    int capR = -1, capC = -1;
//
//    if (backupSrcType == PAWN && sc != ec && backupDstType == NONE) {
//        if (inBounds(sr, ec) && boardType[sr][ec] == PAWN && boardEnPassant[sr][ec] && boardColor[sr][ec] != backupSrcColor) {
//            capturedEP = true;
//            capR = sr; capC = ec;
//            capturedEPBackupType = boardType[capR][capC];
//            capturedEPBackupColor = boardColor[capR][capC];
//            capturedEPBackupMoved = boardMoved[capR][capC];
//            capturedEPBackupEP = boardEnPassant[capR][capC];
//            boardType[capR][capC] = NONE;
//            boardColor[capR][capC] = NO_COLOR;
//            boardMoved[capR][capC] = false;
//            boardEnPassant[capR][capC] = false;
//        }
//    }
//
//    // perform move
//    boardType[er][ec] = boardType[sr][sc];
//    boardColor[er][ec] = boardColor[sr][sc];
//    boardMoved[er][ec] = boardMoved[sr][sc];
//    boardEnPassant[er][ec] = boardEnPassant[sr][sc];
//
//    boardType[sr][sc] = NONE;
//    boardColor[sr][sc] = NO_COLOR;
//    boardMoved[sr][sc] = false;
//    boardEnPassant[sr][sc] = false;
//
//    // handle castling rook move in simulation
//    if (backupSrcType == KING && abs(ec - sc) == 2) {
//        int rookFromC = (ec > sc) ? 7 : 0;
//        int rookToC = (ec > sc) ? ec - 1 : ec + 1;
//        boardType[sr][rookToC] = boardType[sr][rookFromC];
//        boardColor[sr][rookToC] = boardColor[sr][rookFromC];
//        boardMoved[sr][rookToC] = boardMoved[sr][rookFromC];
//        boardEnPassant[sr][rookToC] = boardEnPassant[sr][rookFromC];
//
//        boardType[sr][rookFromC] = NONE;
//        boardColor[sr][rookFromC] = NO_COLOR;
//        boardMoved[sr][rookFromC] = false;
//        boardEnPassant[sr][rookFromC] = false;
//    }
//
//    pair<int, int> kingPos;
//    if (backupSrcType == KING) kingPos = { er,ec };
//    else kingPos = findKing(backupSrcColor);
//
//    bool inCheck = false;
//    if (kingPos.first != -1) inCheck = isSquareAttacked(kingPos.first, kingPos.second, (backupSrcColor == WHITE) ? BLACK : WHITE);
//
//    // undo simulated castling rook move
//    if (backupSrcType == KING && abs(ec - sc) == 2) {
//        int rookFromC = (ec > sc) ? 7 : 0;
//        int rookToC = (ec > sc) ? ec - 1 : ec + 1;
//        boardType[sr][rookFromC] = boardType[sr][rookToC];
//        boardColor[sr][rookFromC] = boardColor[sr][rookToC];
//        boardMoved[sr][rookFromC] = boardMoved[sr][rookToC];
//        boardEnPassant[sr][rookFromC] = boardEnPassant[sr][rookToC];
//
//        boardType[sr][rookToC] = NONE;
//        boardColor[sr][rookToC] = NO_COLOR;
//        boardMoved[sr][rookToC] = false;
//        boardEnPassant[sr][rookToC] = false;
//    }
//
//    // restore
//    boardType[sr][sc] = backupSrcType;
//    boardColor[sr][sc] = backupSrcColor;
//    boardMoved[sr][sc] = backupSrcMoved;
//    boardEnPassant[sr][sc] = backupSrcEP;
//
//    boardType[er][ec] = backupDstType;
//    boardColor[er][ec] = backupDstColor;
//    boardMoved[er][ec] = backupDstMoved;
//    boardEnPassant[er][ec] = backupDstEP;
//
//    if (capturedEP) {
//        boardType[capR][capC] = capturedEPBackupType;
//        boardColor[capR][capC] = capturedEPBackupColor;
//        boardMoved[capR][capC] = capturedEPBackupMoved;
//        boardEnPassant[capR][capC] = capturedEPBackupEP;
//    }
//
//    return inCheck;
//}
//
//void clearAllEnPassantFlags() {
//    for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++)
//        if (boardType[r][c] == PAWN) boardEnPassant[r][c] = false;
//}
//
//bool makeMoveIfLegal(int sr, int sc, int er, int ec) {
//    if (!inBounds(sr, sc) || !inBounds(er, ec)) return false;
//    PieceType pType = boardType[sr][sc];
//    PieceColor pColor = boardColor[sr][sc];
//    if (pType == NONE || pColor != activeColor) return false;
//    if (!isLegalMoveInternal(sr, sc, er, ec, activeColor)) return false;
//    if (wouldLeaveKingInCheck(sr, sc, er, ec)) return false;
//
//    bool capturedEP = false;
//    PieceType capturedEPBackupType = NONE;
//    PieceColor capturedEPBackupColor = NO_COLOR;
//    bool capturedEPBackupMoved = false;
//    bool capturedEPBackupEP = false;
//    int capR = -1, capC = -1;
//
//    bool normalCapture = (boardType[er][ec] != NONE && boardColor[er][ec] != pColor);
//
//    // en-passant capture
//    if (pType == PAWN && sc != ec && boardType[er][ec] == NONE) {
//        if (inBounds(sr, ec) && boardType[sr][ec] == PAWN && boardEnPassant[sr][ec] && boardColor[sr][ec] != pColor) {
//            capturedEP = true;
//            capR = sr; capC = ec;
//            capturedEPBackupType = boardType[capR][capC];
//            capturedEPBackupColor = boardColor[capR][capC];
//            capturedEPBackupMoved = boardMoved[capR][capC];
//            capturedEPBackupEP = boardEnPassant[capR][capC];
//            boardType[capR][capC] = NONE;
//            boardColor[capR][capC] = NO_COLOR;
//            boardMoved[capR][capC] = false;
//            boardEnPassant[capR][capC] = false;
//        }
//    }
//
//    if (normalCapture) {
//        lastCapR = er;
//        lastCapC = ec;
//        lastCapType = boardType[er][ec];
//        lastCapColor = boardColor[er][ec];
//        capTimer = CAP_FRAMES;
//    }
//    if (capturedEP) {
//        lastCapR = capR;
//        lastCapC = capC;
//        lastCapType = capturedEPBackupType;
//        lastCapColor = capturedEPBackupColor;
//        capTimer = CAP_FRAMES;
//    }
//
//    // castling rook reposition
//    if (pType == KING && abs(ec - sc) == 2) {
//        int rookFromC = (ec > sc) ? 7 : 0;
//        int rookToC = (ec > sc) ? ec - 1 : ec + 1;
//        boardType[sr][rookToC] = boardType[sr][rookFromC];
//        boardColor[sr][rookToC] = boardColor[sr][rookFromC];
//        boardMoved[sr][rookToC] = boardMoved[sr][rookFromC];
//        boardEnPassant[sr][rookToC] = boardEnPassant[sr][rookFromC];
//
//        boardType[sr][rookFromC] = NONE;
//        boardColor[sr][rookFromC] = NO_COLOR;
//        boardMoved[sr][rookFromC] = false;
//        boardEnPassant[sr][rookFromC] = false;
//        boardMoved[sr][rookToC] = true;
//    }
//
//    // perform move
//    boardType[er][ec] = boardType[sr][sc];
//    boardColor[er][ec] = boardColor[sr][sc];
//    boardMoved[er][ec] = true;
//    boardEnPassant[er][ec] = boardEnPassant[sr][sc];
//
//    boardType[sr][sc] = NONE;
//    boardColor[sr][sc] = NO_COLOR;
//    boardMoved[sr][sc] = false;
//    boardEnPassant[sr][sc] = false;
//
//    clearAllEnPassantFlags();
//    if (boardType[er][ec] == PAWN && abs(er - sr) == 2) boardEnPassant[er][ec] = true;
//
//    if (boardType[er][ec] == PAWN && ((boardColor[er][ec] == WHITE && er == 0) || (boardColor[er][ec] == BLACK && er == 7)))
//        boardType[er][ec] = QUEEN;
//
//    activeColor = (activeColor == WHITE) ? BLACK : WHITE;
//
//    return true;
//}
//
//bool anyLegalMoveExists(PieceColor color) {
//    for (int sr = 0; sr < 8; sr++) for (int sc = 0; sc < 8; sc++) {
//        if (boardType[sr][sc] == NONE || boardColor[sr][sc] != color) continue;
//        for (int er = 0; er < 8; er++) for (int ec = 0; ec < 8; ec++) {
//            if (sr == er && sc == ec) continue;
//            if (!isLegalMoveInternal(sr, sc, er, ec, color)) continue;
//            if (!wouldLeaveKingInCheck(sr, sc, er, ec)) return true;
//        }
//    }
//    return false;
//}
//
//string pieceName(PieceType t, PieceColor c) {
//    if (t == NONE) return " . ";
//    string s = (c == WHITE) ? "W" : "B";
//    switch (t) {
//    case PAWN: s += "P"; break;
//    case ROOK: s += "R"; break;
//    case KNIGHT: s += "N"; break;
//    case BISHOP: s += "B"; break;
//    case QUEEN: s += "Q"; break;
//    case KING: s += "K"; break;
//    default: s += "?"; break;
//    }
//    return s;
//}
//
//Texture* textureFor(PieceType t, PieceColor c) {
//    if (c == WHITE) {
//        if (t == PAWN) return &texWP;
//        if (t == ROOK) return &texWR;
//        if (t == KNIGHT) return &texWKN;
//        if (t == BISHOP) return &texWB;
//        if (t == QUEEN) return &texWQ;
//        if (t == KING) return &texWK;
//    }
//    else if (c == BLACK) {
//        if (t == PAWN) return &texBP;
//        if (t == ROOK) return &texBR;
//        if (t == KNIGHT) return &texBKN;
//        if (t == BISHOP) return &texBB;
//        if (t == QUEEN) return &texBQ;
//        if (t == KING) return &texBK;
//    }
//    return nullptr;
//}
//
//// --- MAIN ---
//int main() {
//    setupInitialBoard();
//    const int squaresize = 100;
//    const int boxes = 8;
//    RenderWindow window(VideoMode(squaresize * boxes, squaresize * boxes + 40), "Chess with Check/Checkmate/Stalemate");
//    window.setFramerateLimit(60);
//
//    if (!texWP.loadFromFile("images/wp.png") ||
//        !texWR.loadFromFile("images/wr.png") ||
//        !texWKN.loadFromFile("images/wkn.png") ||
//        !texWB.loadFromFile("images/wb.png") ||
//        !texWQ.loadFromFile("images/wq.png") ||
//        !texWK.loadFromFile("images/wk.png") ||
//        !texBP.loadFromFile("images/bp.png") ||
//        !texBR.loadFromFile("images/br.png") ||
//        !texBKN.loadFromFile("images/bkn.png") ||
//        !texBB.loadFromFile("images/bb.png") ||
//        !texBQ.loadFromFile("images/bq.png") ||
//        !texBK.loadFromFile("images/bk.png")) {
//        cout << "Texture load failed!" << endl;
//        return -1;
//    }
//
//    Font font;
//    if (!font.loadFromFile("arial.ttf")) {
//        cout << "Failed to load font arial.ttf" << endl;
//    }
//    Text statusText("", font, 20);
//    statusText.setFillColor(Color::Blue);
//    statusText.setPosition(5, squaresize * boxes + 5);
//
//    bool dragging = false;
//    int dragR = -1, dragC = -1;
//    Vector2f dragOffset;
//
//    // capture-target map (which enemy pieces can be captured by the currently-dragged piece)
//    bool captureTarget[8][8];
//
//    string message = "White to move";
//    string lastMsg = "";
//
//    while (window.isOpen()) {
//        Event e;
//        while (window.pollEvent(e)) {
//            if (e.type == Event::Closed) window.close();
//            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left) {
//                int c = e.mouseButton.x / squaresize;
//                int r = e.mouseButton.y / squaresize;
//                if (inBounds(r, c) && boardType[r][c] != NONE && boardColor[r][c] == activeColor) {
//                    dragging = true;
//                    dragR = r; dragC = c;
//                    dragOffset = Vector2f(e.mouseButton.x - c * squaresize, e.mouseButton.y - r * squaresize);
//                }
//            }
//            if (e.type == Event::MouseButtonReleased && e.mouseButton.button == Mouse::Left) {
//                if (dragging) {
//                    int c = e.mouseButton.x / squaresize;
//                    int r = e.mouseButton.y / squaresize;
//                    if (!makeMoveIfLegal(dragR, dragC, r, c)) {
//                        // illegal, revert (do nothing)
//                    }
//                    dragging = false;
//                    dragR = dragC = -1;
//                }
//            }
//        }
//
//        // compute message using the side-to-move's check/moves
//        PieceColor sideToMove = activeColor;
//        bool sideInCheck = isInCheck(sideToMove);
//        bool sideHasMoves = anyLegalMoveExists(sideToMove);
//
//        if (sideInCheck && !sideHasMoves) {
//            // opponent (the side not to move) delivered checkmate
//            message = (sideToMove == WHITE) ? "Black wins by CHECKMATE!" : "White wins by CHECKMATE!";
//        }
//        else if (!sideInCheck && !sideHasMoves) {
//            message = "STALEMATE! DRAW.";
//        }
//        else {
//            message = (activeColor == WHITE) ? "White to move" : "Black to move";
//            if (sideInCheck) message += " - KING IN CHECK!";
//        }
//
//        statusText.setString(message);
//        if (message != lastMsg) { cout << message << endl; lastMsg = message; }
//
//        // update capture-timer
//        if (capTimer > 0) capTimer--;
//
//        // prepare captureTarget map when dragging
//        for (int rr = 0; rr < 8; rr++) for (int cc = 0; cc < 8; cc++) captureTarget[rr][cc] = false;
//        if (dragging && inBounds(dragR, dragC) && boardType[dragR][dragC] != NONE) {
//            PieceType pType = boardType[dragR][dragC];
//            PieceColor pColor = boardColor[dragR][dragC];
//            for (int er = 0; er < 8; er++) for (int ec = 0; ec < 8; ec++) {
//                if (er == dragR && ec == dragC) continue;
//                if (boardType[er][ec] == NONE) continue; // we want only pieces that can be captured
//                if (boardColor[er][ec] == pColor) continue;
//                if (!isLegalMoveInternal(dragR, dragC, er, ec, pColor)) continue;
//                if (wouldLeaveKingInCheck(dragR, dragC, er, ec)) continue;
//                captureTarget[er][ec] = true;
//            }
//        }
//
//        window.clear(Color::White);
//
//        // draw board squares
//        for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
//            RectangleShape sq(Vector2f(squaresize, squaresize));
//            sq.setPosition(c * squaresize, r * squaresize);
//            sq.setFillColor(((r + c) % 2 == 0) ? Color(240, 217, 181) : Color(181, 136, 99));
//            window.draw(sq);
//        }
//
//        // draw pieces except the one being dragged
//        for (int r = 0; r < 8; r++) for (int c = 0; c < 8; c++) {
//            if (dragging && r == dragR && c == dragC) continue;
//
//            // last captured red flash (takes precedence)
//            if (capTimer > 0 && r == lastCapR && c == lastCapC && lastCapType != NONE) {
//                Texture* capTex = textureFor(lastCapType, lastCapColor);
//                if (capTex) {
//                    Sprite s;
//                    s.setTexture(*capTex);
//                    s.setPosition(c * squaresize, r * squaresize);
//                    s.setColor(Color::Red); // tint captured piece red
//                    window.draw(s);
//                }
//                continue;
//            }
//
//            if (boardType[r][c] == NONE) continue;
//
//            Texture* t = textureFor(boardType[r][c], boardColor[r][c]);
//            if (t) {
//                Sprite s;
//                s.setTexture(*t);
//                s.setPosition(c * squaresize, r * squaresize);
//                if (captureTarget[r][c]) s.setColor(Color::Red); // highlight capturable enemy piece red
//                window.draw(s);
//            }
//        }
//
//        // draw dragged piece following mouse
//        if (dragging && dragR != -1 && dragC != -1) {
//            Texture* t = textureFor(boardType[dragR][dragC], boardColor[dragR][dragC]);
//            if (t) {
//                Sprite s;
//                s.setTexture(*t);
//                Vector2i mp = Mouse::getPosition(window);
//                s.setPosition(mp.x - dragOffset.x, mp.y - dragOffset.y);
//                window.draw(s);
//            }
//        }
//
//        // highlight king if in check 
//        pair<int, int> kingPos = findKing((activeColor == WHITE) ? BLACK : WHITE);
//        if (kingPos.first != -1 && isInCheck((activeColor == WHITE) ? BLACK : WHITE)) {
//            RectangleShape ko(Vector2f(squaresize, squaresize));
//            ko.setPosition(kingPos.second * squaresize, kingPos.first * squaresize);
//            ko.setFillColor(Color(0, 0, 0, 0));
//            ko.setOutlineColor(Color::Red);
//            ko.setOutlineThickness(4.f);
//            window.draw(ko);
//        }
//
//        window.draw(statusText);
//        window.display();
//    }
//    return 0;
//}