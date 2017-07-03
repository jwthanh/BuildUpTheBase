<GameFile>
  <PropertyGroup Name="bank_scene" Type="Scene" ID="c3cd3a87-c5c9-482c-9d54-e2dab7e43098" Version="3.10.0.0" />
  <Content ctype="GameProjectContent">
    <Content>
      <Animation Duration="0" Speed="1.0000" />
      <ObjectData Name="Scene" Tag="32" ctype="GameNodeObjectData">
        <Size X="960.0000" Y="640.0000" />
        <Children>
          <AbstractNodeData Name="panel" ActionTag="651648077" Tag="39" IconVisible="False" TopMargin="0.0003" BottomMargin="-0.0003" TouchEnable="True" ClipAble="False" BackColorAlpha="102" ComboBoxIndex="2" ColorAngle="270.0000" Scale9Width="1" Scale9Height="1" ctype="PanelObjectData">
            <Size X="960.0000" Y="640.0000" />
            <Children>
              <AbstractNodeData Name="back_btn" ActionTag="25721339" Tag="40" IconVisible="False" LeftMargin="35.0000" RightMargin="795.0000" TopMargin="500.0000" BottomMargin="40.0000" TouchEnable="True" FontSize="24" ButtonText="Back" Scale9Enable="True" LeftEage="15" RightEage="15" TopEage="11" BottomEage="11" Scale9OriginX="15" Scale9OriginY="11" Scale9Width="16" Scale9Height="14" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="ButtonObjectData">
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
              <AbstractNodeData Name="title_lbl" ActionTag="127619952" Tag="41" IconVisible="False" LeftMargin="411.4999" RightMargin="411.5001" TopMargin="34.2877" BottomMargin="581.7123" LabelText="The Bank" ctype="TextBMFontObjectData">
                <Size X="137.0000" Y="24.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="479.9999" Y="593.7123" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.5000" Y="0.9277" />
                <PreSize X="0.1427" Y="0.0375" />
                <LabelBMFontFile_CNB Type="Normal" Path="pixelmix_24x2.fnt" Plist="" />
              </AbstractNodeData>
              <AbstractNodeData Name="desc_lbl" ActionTag="-1682992427" Tag="42" IconVisible="False" LeftMargin="-5.5001" RightMargin="-5.4999" TopMargin="87.2877" BottomMargin="528.7123" LabelText="Stores excess coins for later. Upgrade at the Marketplace" ctype="TextBMFontObjectData">
                <Size X="971.0000" Y="24.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="479.9999" Y="540.7123" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.5000" Y="0.8449" />
                <PreSize X="1.0115" Y="0.0375" />
                <LabelBMFontFile_CNB Type="Normal" Path="pixelmix_24x2.fnt" Plist="" />
              </AbstractNodeData>
              <AbstractNodeData Name="bank_listview" ActionTag="302156767" Tag="43" IconVisible="False" LeftMargin="182.9997" RightMargin="-622.9998" TopMargin="137.0004" BottomMargin="-422.0004" TouchEnable="True" ClipAble="True" BackColorAlpha="102" ComboBoxIndex="1" ColorAngle="90.0000" Scale9Width="1" Scale9Height="1" IsBounceEnabled="True" ScrollDirectionType="0" DirectionType="Vertical" ctype="ListViewObjectData">
                <Size X="1400.0000" Y="925.0000" />
                <AnchorPoint ScaleY="1.0000" />
                <Position X="182.9997" Y="502.9996" />
                <Scale ScaleX="0.5000" ScaleY="0.5000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.1906" Y="0.7859" />
                <PreSize X="1.4583" Y="1.4453" />
                <SingleColor A="255" R="150" G="150" B="255" />
                <FirstColor A="255" R="150" G="150" B="255" />
                <EndColor A="255" R="255" G="255" B="255" />
                <ColorVector ScaleY="1.0000" />
              </AbstractNodeData>
              <AbstractNodeData Name="total_coins_panel" ActionTag="-176246686" Tag="199" IconVisible="False" LeftMargin="14.0000" RightMargin="786.0000" TopMargin="174.0000" BottomMargin="380.0000" TouchEnable="True" ClipAble="False" BackColorAlpha="39" ComboBoxIndex="2" ColorAngle="270.0000" Scale9Width="1" Scale9Height="1" ctype="PanelObjectData">
                <Size X="160.0000" Y="86.0000" />
                <Children>
                  <AbstractNodeData Name="total_coins_header" ActionTag="-1245448573" Tag="201" IconVisible="False" LeftMargin="35.5000" RightMargin="35.5000" TopMargin="8.0000" BottomMargin="54.0000" LabelText="Coins:" ctype="TextBMFontObjectData">
                    <Size X="89.0000" Y="24.0000" />
                    <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                    <Position X="80.0000" Y="66.0000" />
                    <Scale ScaleX="1.0000" ScaleY="1.0000" />
                    <CColor A="255" R="255" G="255" B="255" />
                    <PrePosition X="0.5000" Y="0.7674" />
                    <PreSize X="0.5562" Y="0.2791" />
                    <LabelBMFontFile_CNB Type="Normal" Path="pixelmix_24x2.fnt" Plist="" />
                  </AbstractNodeData>
                  <AbstractNodeData Name="total_coins_count_lbl" ActionTag="1234377149" Tag="202" IconVisible="False" LeftMargin="-10.0000" RightMargin="-12.0000" TopMargin="51.0000" BottomMargin="11.0000" LabelText="123,456Quax" ctype="TextBMFontObjectData">
                    <Size X="182.0000" Y="24.0000" />
                    <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                    <Position X="81.0000" Y="23.0000" />
                    <Scale ScaleX="1.0000" ScaleY="1.0000" />
                    <CColor A="255" R="255" G="255" B="255" />
                    <PrePosition X="0.5063" Y="0.2674" />
                    <PreSize X="1.1375" Y="0.2791" />
                    <LabelBMFontFile_CNB Type="Normal" Path="pixelmix_24x2.fnt" Plist="" />
                  </AbstractNodeData>
                  <AbstractNodeData Name="total_coins_added" ActionTag="973881901" VisibleForFrame="False" Tag="78" IconVisible="False" LeftMargin="7.0000" RightMargin="9.0000" TopMargin="-27.0000" BottomMargin="97.0000" LabelText="+ 123,456Quax" ctype="TextBMFontObjectData">
                    <Size X="144.0000" Y="16.0000" />
                    <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                    <Position X="79.0000" Y="105.0000" />
                    <Scale ScaleX="1.0000" ScaleY="1.0000" />
                    <CColor A="255" R="255" G="255" B="255" />
                    <PrePosition X="0.4938" Y="1.2209" />
                    <PreSize X="0.9000" Y="0.1860" />
                    <LabelBMFontFile_CNB Type="Normal" Path="pixelmix_16x2.fnt" Plist="" />
                  </AbstractNodeData>
                </Children>
                <AnchorPoint />
                <Position X="14.0000" Y="380.0000" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.0146" Y="0.5938" />
                <PreSize X="0.1667" Y="0.1344" />
                <SingleColor A="255" R="150" G="200" B="255" />
                <FirstColor A="255" R="150" G="200" B="255" />
                <EndColor A="255" R="255" G="255" B="255" />
                <ColorVector ScaleX="0.0000" ScaleY="-1.0000" />
              </AbstractNodeData>
              <AbstractNodeData Name="banked_coins_panel" ActionTag="-2096377961" Tag="203" IconVisible="False" LeftMargin="14.0000" RightMargin="786.0000" TopMargin="334.0000" BottomMargin="220.0000" TouchEnable="True" ClipAble="False" BackColorAlpha="39" ComboBoxIndex="2" ColorAngle="270.0000" Scale9Width="1" Scale9Height="1" ctype="PanelObjectData">
                <Size X="160.0000" Y="86.0000" />
                <Children>
                  <AbstractNodeData Name="banked_coins_header" ActionTag="736078382" Tag="204" IconVisible="False" LeftMargin="25.0000" RightMargin="25.0000" TopMargin="8.0000" BottomMargin="54.0000" LabelText="Banked:" ctype="TextBMFontObjectData">
                    <Size X="110.0000" Y="24.0000" />
                    <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                    <Position X="80.0000" Y="66.0000" />
                    <Scale ScaleX="1.0000" ScaleY="1.0000" />
                    <CColor A="255" R="255" G="255" B="255" />
                    <PrePosition X="0.5000" Y="0.7674" />
                    <PreSize X="0.6875" Y="0.2791" />
                    <LabelBMFontFile_CNB Type="Normal" Path="pixelmix_24x2.fnt" Plist="" />
                  </AbstractNodeData>
                  <AbstractNodeData Name="banked_coins_count_lbl" ActionTag="-2052060896" Tag="205" IconVisible="False" LeftMargin="-10.0000" RightMargin="-12.0000" TopMargin="51.0000" BottomMargin="11.0000" LabelText="123,456Quax" ctype="TextBMFontObjectData">
                    <Size X="182.0000" Y="24.0000" />
                    <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                    <Position X="81.0000" Y="23.0000" />
                    <Scale ScaleX="1.0000" ScaleY="1.0000" />
                    <CColor A="255" R="255" G="255" B="255" />
                    <PrePosition X="0.5063" Y="0.2674" />
                    <PreSize X="1.1375" Y="0.2791" />
                    <LabelBMFontFile_CNB Type="Normal" Path="pixelmix_24x2.fnt" Plist="" />
                  </AbstractNodeData>
                  <AbstractNodeData Name="banked_coins_added" ActionTag="1202857193" VisibleForFrame="False" Tag="77" IconVisible="False" LeftMargin="7.0000" RightMargin="9.0000" TopMargin="-27.0000" BottomMargin="97.0000" LabelText="+ 123,456Quax" ctype="TextBMFontObjectData">
                    <Size X="144.0000" Y="16.0000" />
                    <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                    <Position X="79.0000" Y="105.0000" />
                    <Scale ScaleX="1.0000" ScaleY="1.0000" />
                    <CColor A="255" R="255" G="255" B="255" />
                    <PrePosition X="0.4938" Y="1.2209" />
                    <PreSize X="0.9000" Y="0.1860" />
                    <LabelBMFontFile_CNB Type="Normal" Path="pixelmix_16x2.fnt" Plist="" />
                  </AbstractNodeData>
                </Children>
                <AnchorPoint />
                <Position X="14.0000" Y="220.0000" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.0146" Y="0.3438" />
                <PreSize X="0.1667" Y="0.1344" />
                <SingleColor A="255" R="150" G="200" B="255" />
                <FirstColor A="255" R="150" G="200" B="255" />
                <EndColor A="255" R="255" G="255" B="255" />
                <ColorVector ScaleX="0.0000" ScaleY="-1.0000" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint ScaleY="1.0000" />
            <Position Y="639.9997" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition Y="1.0000" />
            <PreSize X="1.0000" Y="1.0000" />
            <SingleColor A="255" R="150" G="200" B="255" />
            <FirstColor A="255" R="150" G="200" B="255" />
            <EndColor A="255" R="255" G="255" B="255" />
            <ColorVector ScaleX="0.0000" ScaleY="-1.0000" />
          </AbstractNodeData>
        </Children>
      </ObjectData>
    </Content>
  </Content>
</GameFile>