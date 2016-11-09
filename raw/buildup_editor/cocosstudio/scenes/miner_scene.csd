<GameFile>
  <PropertyGroup Name="miner_scene" Type="Scene" ID="e4edb5e7-7961-40a7-96ca-802dfe1339b7" Version="3.10.0.0" />
  <Content ctype="GameProjectContent">
    <Content>
      <Animation Duration="0" Speed="1.0000" />
      <ObjectData Name="miner_scene" Tag="223" ctype="GameNodeObjectData">
        <Size X="960.0000" Y="640.0000" />
        <Children>
          <AbstractNodeData Name="tilemap_nav" ActionTag="-256894720" Tag="224" IconVisible="True" LeftMargin="173.1085" RightMargin="786.8915" TopMargin="136.8814" BottomMargin="503.1186" StretchWidthEnable="False" StretchHeightEnable="False" InnerActionSpeed="1.0000" CustomSizeEnabled="False" ctype="ProjectNodeObjectData">
            <Size X="0.0000" Y="0.0000" />
            <AnchorPoint />
            <Position X="173.1085" Y="503.1186" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.1803" Y="0.7861" />
            <PreSize X="0.0000" Y="0.0000" />
            <FileData Type="Normal" Path="buttons/tilemap_nav.csd" Plist="" />
          </AbstractNodeData>
          <AbstractNodeData Name="tilemap" ActionTag="1667513557" Tag="232" IconVisible="True" LeftMargin="643.8708" RightMargin="316.1292" TopMargin="303.9103" BottomMargin="336.0897" ctype="SingleNodeObjectData">
            <Size X="0.0000" Y="0.0000" />
            <AnchorPoint />
            <Position X="643.8708" Y="336.0897" />
            <Scale ScaleX="9.9000" ScaleY="9.9000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.6707" Y="0.5251" />
            <PreSize X="0.0000" Y="0.0000" />
          </AbstractNodeData>
          <AbstractNodeData Name="back_btn" ActionTag="1232804101" Tag="233" IconVisible="False" LeftMargin="35.0000" RightMargin="795.0000" TopMargin="500.0000" BottomMargin="40.0000" TouchEnable="True" FontSize="24" ButtonText="Back" Scale9Enable="True" LeftEage="15" RightEage="15" TopEage="11" BottomEage="11" Scale9OriginX="15" Scale9OriginY="11" Scale9Width="16" Scale9Height="14" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="ButtonObjectData">
            <Size X="130.0000" Y="100.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="100.0000" Y="90.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.1042" Y="0.1406" />
            <PreSize X="0.1354" Y="0.1563" />
            <FontResource Type="Normal" Path="pixelmix.ttf" Plist="" />
            <TextColor A="255" R="65" G="65" B="70" />
            <DisabledFileData Type="Default" Path="Default/Button_Disable.png" Plist="" />
            <PressedFileData Type="Default" Path="Default/Button_Press.png" Plist="" />
            <NormalFileData Type="Default" Path="Default/Button_Normal.png" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="info_panel" ActionTag="-621482901" Tag="20" IconVisible="False" LeftMargin="760.0000" BottomMargin="440.0000" TouchEnable="True" ClipAble="False" BackColorAlpha="102" ComboBoxIndex="1" ColorAngle="90.0000" Scale9Width="1" Scale9Height="1" ctype="PanelObjectData">
            <Size X="200.0000" Y="200.0000" />
            <Children>
              <AbstractNodeData Name="mine_rails_lbl" ActionTag="-1424291244" Tag="113" IconVisible="False" LeftMargin="-1.0597" RightMargin="1.0597" TopMargin="12.7188" BottomMargin="163.2812" IsCustomSize="True" FontSize="20" LabelText="Mine Rails" HorizontalAlignmentType="HT_Center" OutlineSize="2" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="200.0000" Y="24.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="98.9403" Y="175.2812" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.4947" Y="0.8764" />
                <PreSize X="1.0000" Y="0.1200" />
                <FontResource Type="Normal" Path="pixelmix.ttf" Plist="" />
                <OutlineColor A="255" R="26" G="26" B="26" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
              <AbstractNodeData Name="mine_rails_count" ActionTag="1431279639" Tag="114" IconVisible="False" LeftMargin="-0.0600" RightMargin="0.0600" TopMargin="41.7194" BottomMargin="134.2806" IsCustomSize="True" FontSize="20" LabelText="XXX,XXXQaD" HorizontalAlignmentType="HT_Center" OutlineSize="2" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="200.0000" Y="24.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="99.9400" Y="146.2806" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.4997" Y="0.7314" />
                <PreSize X="1.0000" Y="0.1200" />
                <FontResource Type="Normal" Path="pixelmix.ttf" Plist="" />
                <OutlineColor A="255" R="26" G="26" B="26" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
              <AbstractNodeData Name="mine_depth_lbl" ActionTag="5292265" Tag="141" IconVisible="False" LeftMargin="-1.0597" RightMargin="1.0597" TopMargin="100.7187" BottomMargin="75.2813" IsCustomSize="True" FontSize="20" LabelText="Depth" HorizontalAlignmentType="HT_Center" OutlineSize="2" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="200.0000" Y="24.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="98.9403" Y="87.2813" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.4947" Y="0.4364" />
                <PreSize X="1.0000" Y="0.1200" />
                <FontResource Type="Normal" Path="pixelmix.ttf" Plist="" />
                <OutlineColor A="255" R="26" G="26" B="26" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
              <AbstractNodeData Name="mine_depth_count" ActionTag="1956838979" Tag="142" IconVisible="False" LeftMargin="-0.0597" RightMargin="0.0597" TopMargin="129.7192" BottomMargin="46.2808" IsCustomSize="True" FontSize="20" LabelText="XXX,XXXQaD" HorizontalAlignmentType="HT_Center" OutlineSize="2" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="200.0000" Y="24.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="99.9403" Y="58.2808" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.4997" Y="0.2914" />
                <PreSize X="1.0000" Y="0.1200" />
                <FontResource Type="Normal" Path="pixelmix.ttf" Plist="" />
                <OutlineColor A="255" R="26" G="26" B="26" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint />
            <Position X="760.0000" Y="440.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.7917" Y="0.6875" />
            <PreSize X="0.2083" Y="0.3125" />
            <SingleColor A="255" R="150" G="200" B="255" />
            <FirstColor A="255" R="150" G="200" B="255" />
            <EndColor A="255" R="255" G="255" B="255" />
            <ColorVector ScaleY="1.0000" />
          </AbstractNodeData>
          <AbstractNodeData Name="dig_btn" ActionTag="1624672489" Tag="19" IconVisible="False" LeftMargin="799.0000" RightMargin="31.0000" TopMargin="498.0000" BottomMargin="42.0000" TouchEnable="True" FontSize="24" ButtonText="Dig" Scale9Enable="True" LeftEage="15" RightEage="15" TopEage="11" BottomEage="11" Scale9OriginX="15" Scale9OriginY="11" Scale9Width="16" Scale9Height="14" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="ButtonObjectData">
            <Size X="130.0000" Y="100.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="864.0000" Y="92.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.9000" Y="0.1437" />
            <PreSize X="0.1354" Y="0.1563" />
            <FontResource Type="Normal" Path="pixelmix.ttf" Plist="" />
            <TextColor A="255" R="65" G="65" B="70" />
            <DisabledFileData Type="Default" Path="Default/Button_Disable.png" Plist="" />
            <PressedFileData Type="Default" Path="Default/Button_Press.png" Plist="" />
            <NormalFileData Type="Default" Path="Default/Button_Normal.png" Plist="" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
        </Children>
      </ObjectData>
    </Content>
  </Content>
</GameFile>